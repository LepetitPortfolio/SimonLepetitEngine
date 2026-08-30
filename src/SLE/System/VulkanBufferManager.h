#pragma once

#include <vulkan/vulkan.h>
#include "VulkanDevice.h"
#include "VulkanCommandManager.h"
#include "../common/Vertex.h"

#include <vector>

struct UniformBufferObject;

class VulkanBufferManager 
{
public:
    VulkanBufferManager();
    ~VulkanBufferManager();

    void Initialize(const VulkanDevice& _Device, VulkanCommandManager& _VulkanCommandManager);
    void Cleanup();

    void CreateBuffer(VkDeviceSize _Size, VkBufferUsageFlags _Usage, VkMemoryPropertyFlags _Properties, VkBuffer& _Buffer, VkDeviceMemory& _BufferMemory);
    void CopyBuffer(VkBuffer _SrcBuffer, VkBuffer _DstBuffer, VkDeviceSize _Size);

    void CreateVertexBuffer(const std::vector<StandardVertex>& _Vertices, VkBuffer& _VertexBuffer, VkDeviceMemory& _VertexBufferMemory);
    void CreateIndexBuffer(const std::vector<uint32_t>& _Indices, VkBuffer& _IndexBuffer, VkDeviceMemory& _IndexBufferMemory);
    void CreateUniformBuffer(uint32_t _MaxFramesInFlight, std::vector<VkBuffer>& _UniformBuffers, std::vector<VkDeviceMemory>& _UniformBuffersMemory, std::vector<void*>& _UniformBuffersMapped);

    void DestroyBuffer(VkBuffer& _Buffer, VkDeviceMemory& _BufferMemory);
private:
    const VulkanDevice* m_VulkanDevice = nullptr;
    VulkanCommandManager* m_CommandManager = nullptr;
};