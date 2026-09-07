#include "VulkanCommandManager.h"
#include "VulkanDevice.h"
#include "VulkanStructs.h"
#include <iostream>
#include <stdexcept>
#include <array>

VulkanCommandManager::VulkanCommandManager() 
{
}

VulkanCommandManager::~VulkanCommandManager() 
{
    Cleanup();
}

void VulkanCommandManager::Initialize(const VulkanDevice& _Device, uint32_t _MaxFramesInFlight) 
{
    this->m_VulkanDevice = &_Device;

    createCommandPool();
    createCommandBuffers(_MaxFramesInFlight);
}

void VulkanCommandManager::Cleanup() 
{
    if (m_VulkanDevice && m_CommandPool != VK_NULL_HANDLE) 
    {
        vkDestroyCommandPool(m_VulkanDevice->GetLogicalDevice(), m_CommandPool, nullptr);
        m_CommandPool = VK_NULL_HANDLE;
    }
}

void VulkanCommandManager::createCommandPool() 
{
    QueueFamilyIndices queueFamilyIndices = m_VulkanDevice->FindQueueFamilies(m_VulkanDevice->GetPhysicalDevice());

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

    VkResult result = vkCreateCommandPool(m_VulkanDevice->GetLogicalDevice(), &poolInfo, nullptr, &m_CommandPool);
    if (result != VK_SUCCESS) 
    {
        std::cout << "failed to create command pool - " << result << std::endl;
        throw std::runtime_error("failed to create command pool!");
    }
    else 
    {
        std::cout << "Successfully created command pool - " << result << std::endl;
    }
}

void VulkanCommandManager::createCommandBuffers(uint32_t _MaxFramesInFlight) 
{
    m_CommandBuffers.resize(_MaxFramesInFlight);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

    VkResult result = vkAllocateCommandBuffers(m_VulkanDevice->GetLogicalDevice(), &allocInfo, m_CommandBuffers.data());
    if (result != VK_SUCCESS) 
    {
        std::cout << "failed to create command buffer - " << result << std::endl;
        throw std::runtime_error("failed to allocate command buffers!");
    }
    else 
    {
        std::cout << "successfully created command buffer - " << result << std::endl;
    }
}

VkCommandBuffer VulkanCommandManager::BeginSingleTimeCommands() 
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_VulkanDevice->GetLogicalDevice(), &allocInfo, &commandBuffer);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanCommandManager::EndSingleTimeCommands(VkCommandBuffer commandBuffer) 
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_VulkanDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_VulkanDevice->GetGraphicsQueue());

    vkFreeCommandBuffers(m_VulkanDevice->GetLogicalDevice(), m_CommandPool, 1, &commandBuffer);
}

void VulkanCommandManager::ResetCommandBuffer(uint32_t frameIndex) 
{
    vkResetCommandBuffer(m_CommandBuffers[frameIndex], 0);
}

void VulkanCommandManager::BeginCommandBuffer(VkCommandBuffer _CommandBuffer)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkResult result = vkBeginCommandBuffer(_CommandBuffer, &beginInfo);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}

void VulkanCommandManager::BeginRenderPass( VkCommandBuffer _CommandBuffer, VkRenderPass _RenderPass, VkFramebuffer _Framebuffer, VkExtent2D _Extent)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _RenderPass;
    renderPassInfo.framebuffer = _Framebuffer;
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = _Extent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandManager::EndRenderPass(VkCommandBuffer _CommandBuffer)
{
    vkCmdEndRenderPass(_CommandBuffer);
}

void VulkanCommandManager::EndCommandBuffer(VkCommandBuffer _CommandBuffer)
{
    VkResult result = vkEndCommandBuffer(_CommandBuffer);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to end recording command buffer!");
    }
}

void VulkanCommandManager::RecordCommandBuffer(VkCommandBuffer _CommandBuffer, uint32_t _ImageIndex, VkRenderPass _RenderPass,VkFramebuffer _Framebuffer, 
                                               VkExtent2D _Extent, VkPipeline _Pipeline, VkPipelineLayout _PipelineLayout, VkBuffer _VertexBuffer, 
                                               VkBuffer _IndexBuffer, const std::vector<VkDescriptorSet>& _DescriptorSets, uint32_t _CurrentFrame, uint32_t _IndexCount)
{
    if (_DescriptorSets.empty() || _DescriptorSets.size() <= _CurrentFrame)
    {
        throw std::runtime_error("invalid descriptor set for current frame!");
    }

    vkCmdBindPipeline(_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _Pipeline);

    VkBuffer vertexBuffers[] = { _VertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    // Diagnostic mode: the vertex shader generates a triangle from gl_VertexIndex,
    // so the mesh vertex/index buffers are intentionally not used for the draw.
    vkCmdBindVertexBuffers(_CommandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(_CommandBuffer, _IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(_Extent.width);
    viewport.height = static_cast<float>(_Extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(_CommandBuffer, 0, 1, &viewport);
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = _Extent;
    vkCmdSetScissor(_CommandBuffer, 0, 1, &scissor);

    vkCmdBindDescriptorSets(_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _PipelineLayout, 0, 1, &_DescriptorSets[_CurrentFrame], 0, nullptr);

    vkCmdDrawIndexed(_CommandBuffer, static_cast<uint32_t>(_IndexCount), 1, 0, 0, 0);
}

