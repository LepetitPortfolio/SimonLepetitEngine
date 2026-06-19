#pragma once
#include "VulkanPlatform.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <string>
#include <vector>

class SwapChain
{
public :

	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

	SwapChain(VulkanPlatform& _VulkanPlatform, VkExtent2D _WindowExtent);
	SwapChain(VulkanPlatform& _VulkanPlatform, VkExtent2D _WindowExtent, std::shared_ptr<SwapChain> _Previous);

	~SwapChain();

	SwapChain(const SwapChain&) = delete;
	SwapChain& operator=(const SwapChain&) = delete;

	VkFramebuffer GetFrameBuffer(int index) { return m_SwapChainFramebuffers[index]; }
	VkRenderPass GetRenderPass() { return m_RenderPass; }
	VkImageView GetImageView(int index) { return m_SwapChainImageViews[index]; }
	size_t ImageCount() { return m_SwapChainImages.size(); }
	VkFormat GetSwapChainImageFormat() { return m_SwapChainImageFormat; }
	VkExtent2D GetSwapChainExtent() { return m_SwapChainExtent; }
	uint32_t Width() { return m_SwapChainExtent.width; }
	uint32_t Height() { return m_SwapChainExtent.height; }

	float ExtentAspectRatio();

	VkFormat FindDepthFormat();

	VkResult AcquireNextImage(uint32_t* _ImageIndex);
	VkResult SubmitCommandBuffers(const VkCommandBuffer* _Buffers, uint32_t* _ImageIndex);

	bool CompareSwapFormats(const SwapChain& _SwapChain);

private:

	VkFormat m_SwapChainImageFormat;
	VkFormat m_SwapChainDepthFormat;
	VkExtent2D m_SwapChainExtent;

	std::vector<VkFramebuffer> m_SwapChainFramebuffers;
	VkRenderPass m_RenderPass;

	std::vector<VkImage> m_DepthImages;
	std::vector<VkDeviceMemory> m_DepthImageMemorys;
	std::vector<VkImageView> m_DepthImageViews;
	std::vector<VkImage> m_SwapChainImages;
	std::vector<VkImageView> m_SwapChainImageViews;

	VulkanPlatform& m_VulkanPlatform;
	VkExtent2D m_WindowExtent;
	VkSwapchainKHR m_SwapChain;
	std::shared_ptr<SwapChain> m_OldSwapChain;

	std::vector<VkSemaphore> m_ImageAvailableSemaphores;
	std::vector<VkSemaphore> m_RenderFinishedSemaphores;
	std::vector<VkFence> m_InFlightFences;
	std::vector<VkFence> m_ImagesInFlight;
	size_t m_CurrentFrame = 0;

	void Init();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateDepthResources();
	void CreateRenderPass();
	void CreateFramebuffers();
	void CreateSyncObjects();

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(	const std::vector<VkSurfaceFormatKHR>& _AvailableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(	const std::vector<VkPresentModeKHR>& _AvailablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& _Capabilities);
};