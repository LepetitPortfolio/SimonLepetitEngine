#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class VulkanDevice;

class VulkanCommandManager 
{
public:
    VulkanCommandManager();
    ~VulkanCommandManager();

    void Initialize(const VulkanDevice& _Device, uint32_t _MaxFramesInFlight);
    void Cleanup();

    VkCommandPool GetCommandPool() const { return m_CommandPool; }
    const std::vector<VkCommandBuffer>& GetCommandBuffers() const { return m_CommandBuffers; }
    VkCommandBuffer GetCommandBuffer(uint32_t frameIndex) const { return m_CommandBuffers[frameIndex]; }

    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer _CommandBuffer);

    void ResetCommandBuffer(uint32_t _FrameIndex);
    void BeginCommandBuffer(VkCommandBuffer _CommandBuffer);
    void BeginRenderPass(VkCommandBuffer _CommandBuffer, VkRenderPass _RenderPass, VkFramebuffer _Framebuffer, VkExtent2D _Extent);
    void EndRenderPass(VkCommandBuffer _CommandBuffer);
    void EndCommandBuffer(VkCommandBuffer _CommandBuffer);
    void RecordCommandBuffer(VkCommandBuffer _CommandBuffer, uint32_t _ImageIndex, VkRenderPass _RenderPass, VkFramebuffer _Framebuffer, VkExtent2D _Extent, 
                             VkPipeline _Pipeline, VkPipelineLayout _PipelineLayout, VkBuffer _VertexBuffer, VkBuffer _IndexBuffer, 
                             const std::vector<VkDescriptorSet>& _DescriptorSets, uint32_t _CurrentFrame, uint32_t _IndexCount);

private:
    const VulkanDevice* m_VulkanDevice = nullptr;
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_CommandBuffers;

    void createCommandPool();
    void createCommandBuffers(uint32_t _MaxFramesInFlight);
};