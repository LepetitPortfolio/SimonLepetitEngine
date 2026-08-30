#include "VulkanBufferManager.h"
#include "VulkanStructs.h"

#include <iostream>
#include <stdexcept>
#include <cstring>

VulkanBufferManager::VulkanBufferManager() {}

VulkanBufferManager::~VulkanBufferManager() 
{
    Cleanup();
}

void VulkanBufferManager::Initialize(const VulkanDevice& _Device, VulkanCommandManager& _VulkanCommandManager) 
{
    m_VulkanDevice = &_Device;
    m_CommandManager = &_VulkanCommandManager;
}

void VulkanBufferManager::Cleanup() 
{

}

void VulkanBufferManager::CreateBuffer(VkDeviceSize _Size, VkBufferUsageFlags _Usage, VkMemoryPropertyFlags _Properties, VkBuffer& _Buffer, VkDeviceMemory& _BufferMemory) 
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = _Size;
    bufferInfo.usage = _Usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_VulkanDevice->GetLogicalDevice(), &bufferInfo, nullptr, &_Buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create _Buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_VulkanDevice->GetLogicalDevice(), _Buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_VulkanDevice->FindMemoryType(memRequirements.memoryTypeBits, _Properties);

    if (vkAllocateMemory(m_VulkanDevice->GetLogicalDevice(), &allocInfo, nullptr, &_BufferMemory) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate _Buffer memory!");
    }

    vkBindBufferMemory(m_VulkanDevice->GetLogicalDevice(), _Buffer, _BufferMemory, 0);
}

void VulkanBufferManager::CopyBuffer(VkBuffer _SrcBuffer, VkBuffer _DstBuffer, VkDeviceSize _Size) 
{
    VkCommandBuffer commandBuffer = m_CommandManager->BeginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = _Size;
    vkCmdCopyBuffer(commandBuffer, _SrcBuffer, _DstBuffer, 1, &copyRegion);

    m_CommandManager->EndSingleTimeCommands(commandBuffer);
}


void VulkanBufferManager::CreateUniformBuffer(uint32_t maxFramesInFlight, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void*>& uniformBuffersMapped) 
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(maxFramesInFlight);
    uniformBuffersMemory.resize(maxFramesInFlight);
    uniformBuffersMapped.resize(maxFramesInFlight);

    for (size_t i = 0; i < maxFramesInFlight; i++) 
    {
        CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
        vkMapMemory(m_VulkanDevice->GetLogicalDevice(), uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
    }
}

void VulkanBufferManager::CreateIndexBuffer(const std::vector<uint32_t>& _Indices, VkBuffer& _IndexBuffer, VkDeviceMemory& _IndexBufferMemory) 
{    
    VkDeviceSize bufferSize = sizeof(_Indices[0]) * _Indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    void* data;
    vkMapMemory(m_VulkanDevice->GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, _Indices.data(), (size_t)bufferSize);
    vkUnmapMemory(m_VulkanDevice->GetLogicalDevice(), stagingBufferMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _IndexBuffer, _IndexBufferMemory);
    CopyBuffer(stagingBuffer, _IndexBuffer, bufferSize);

    vkDestroyBuffer(m_VulkanDevice->GetLogicalDevice(), stagingBuffer, nullptr);
    vkFreeMemory(m_VulkanDevice->GetLogicalDevice(), stagingBufferMemory, nullptr);
}

void VulkanBufferManager::CreateVertexBuffer(const std::vector<StandardVertex>& _Vertices, VkBuffer& _VertexBuffer, VkDeviceMemory& _VertexBufferMemory) 
{
    VkDeviceSize bufferSize = sizeof(_Vertices[0]) * _Vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    void* data;
    vkMapMemory(m_VulkanDevice->GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, _Vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(m_VulkanDevice->GetLogicalDevice(), stagingBufferMemory);
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _VertexBuffer, _VertexBufferMemory);
    CopyBuffer(stagingBuffer, _VertexBuffer, bufferSize);

    vkDestroyBuffer(m_VulkanDevice->GetLogicalDevice(), stagingBuffer, nullptr);
    vkFreeMemory(m_VulkanDevice->GetLogicalDevice(), stagingBufferMemory, nullptr);
}

void VulkanBufferManager::DestroyBuffer(VkBuffer& _Buffer, VkDeviceMemory& _BufferMemory) 
{
    if (_Buffer != VK_NULL_HANDLE) 
    {
        vkDestroyBuffer(m_VulkanDevice->GetLogicalDevice(), _Buffer, nullptr);
        _Buffer = VK_NULL_HANDLE;
    }
    if (_BufferMemory != VK_NULL_HANDLE) 
    {
        vkFreeMemory(m_VulkanDevice->GetLogicalDevice(), _BufferMemory, nullptr);
        _BufferMemory = VK_NULL_HANDLE;
    }
}