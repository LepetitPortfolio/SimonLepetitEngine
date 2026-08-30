#pragma once
#include <vulkan/vulkan.h>

#include <cassert>
#include <memory>
#include <vector>

class VulkanDevice;
class VulkanSwapchain;

class VulkanRenderer
{
public:
	VulkanRenderer();
	~VulkanRenderer();

	VulkanRenderer(const VulkanRenderer&) = delete;
	VulkanRenderer& operator=(const VulkanRenderer&) = delete;

	void Initialize(VulkanDevice& _Device, VulkanSwapchain& _Swapchain);
	void Cleanup();
	void RecreatePipelines();

	VkRenderPass GetRenderPass() const { return m_RenderPass; }
	VkCommandBuffer GetCurrentCommandBuffer() const;

	float GetAspectRatio() const;

	int GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

	bool HasStencilComponent(VkFormat _Format);
	VkFormat FindDepthFormat();

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& _Candidates, VkImageTiling _Tiling, VkFormatFeatureFlags _Features);

private:

	VulkanDevice* m_VulkanDevice = nullptr;
	VulkanSwapchain* m_VulkanSwapchain = nullptr;

	VkRenderPass m_RenderPass = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_CommandBuffers;

	uint32_t m_CurrentImageIndex;
	int m_CurrentFrameIndex{ 0 };
	bool m_IsFrameStarted{ false };

	void CreateRenderPass(VkFormat _SwapChainImageFormat);
};
