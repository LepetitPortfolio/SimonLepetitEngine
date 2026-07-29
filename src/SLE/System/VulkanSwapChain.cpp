#include "VulkanSwapChain.h"

#include "../Core/GlobalFunctionLibrary.h"
#include "VulkanStructs.h"

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

VulkanSwapChain::VulkanSwapChain(VkExtent2D _WindowExtent): m_WindowExtent(_WindowExtent)
{
	Init();
}

VulkanSwapChain::VulkanSwapChain(VkExtent2D _WindowExtent, std::shared_ptr<VulkanSwapChain> _Previous) : m_WindowExtent(_WindowExtent), m_OldSwapChain(_Previous)
{
	Init();
	m_OldSwapChain = nullptr;
}

VulkanSwapChain::~VulkanSwapChain()
{
	for (auto imageView : m_SwapChainImageViews)
	{
		vkDestroyImageView(m_VulkanPlatform->GetDevice(), imageView, nullptr);
	}
	m_SwapChainImageViews.clear();

	if (m_SwapChain != nullptr)
	{
		vkDestroySwapchainKHR(m_VulkanPlatform->GetDevice(), m_SwapChain, nullptr);
		m_SwapChain = nullptr;
	}

	for (int colorImagesIndex = 0; colorImagesIndex < m_ColorImage.size(); colorImagesIndex++)
	{
		vkDestroyImageView(m_VulkanPlatform->GetDevice(), m_ColorImageView[colorImagesIndex], nullptr);
		vkDestroyImage(m_VulkanPlatform->GetDevice(), m_ColorImage[colorImagesIndex], nullptr);
		vkFreeMemory(m_VulkanPlatform->GetDevice(), m_ColorImageMemory[colorImagesIndex], nullptr);
	}

	for (int depthImagesIndex = 0; depthImagesIndex < m_DepthImages.size(); depthImagesIndex++)
	{
		vkDestroyImageView(m_VulkanPlatform->GetDevice(), m_DepthImageViews[depthImagesIndex], nullptr);
		vkDestroyImage(m_VulkanPlatform->GetDevice(), m_DepthImages[depthImagesIndex], nullptr);
		vkFreeMemory(m_VulkanPlatform->GetDevice(), m_DepthImageMemorys[depthImagesIndex], nullptr);
	}

	for (auto framebuffer : m_SwapChainFramebuffers)
	{
		vkDestroyFramebuffer(m_VulkanPlatform->GetDevice(), framebuffer, nullptr);
	}

	vkDestroyRenderPass(m_VulkanPlatform->GetDevice(), m_RenderPass, nullptr);

	// cleanup synchronization objects
	for (size_t FrameInFlightIndex = 0; FrameInFlightIndex < MAX_FRAMES_IN_FLIGHT; FrameInFlightIndex++)
	{
		vkDestroySemaphore(m_VulkanPlatform->GetDevice(), m_RenderFinishedSemaphores[FrameInFlightIndex], nullptr);
		vkDestroySemaphore(m_VulkanPlatform->GetDevice(), m_ImageAvailableSemaphores[FrameInFlightIndex], nullptr);
		vkDestroyFence(m_VulkanPlatform->GetDevice(), m_InFlightFences[FrameInFlightIndex], nullptr);
	}

	m_VulkanPlatform = nullptr;

}

void VulkanSwapChain::Init()
{
	m_VulkanPlatform = GlobalFunctionLibrary::GetVulkanPlatform();
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateDepthResources();
	CreateColorResources();
	CreateFramebuffers();
	CreateSyncObjects();
}

void VulkanSwapChain::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = m_VulkanPlatform->GetSwapChainSupport();

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.Capabilities);

	uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
	if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount) 
	{
		imageCount = swapChainSupport.Capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_VulkanPlatform->GetSurface();

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = m_VulkanPlatform->FindPhysicalQueueFamilies();
	uint32_t queueFamilyIndices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value()};

	if (indices.GraphicsFamily != indices.PresentFamily) 
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else 
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;     
		createInfo.pQueueFamilyIndices = nullptr; 
	}

	createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = m_OldSwapChain == nullptr ? VK_NULL_HANDLE : m_OldSwapChain->m_SwapChain;

	if (vkCreateSwapchainKHR(m_VulkanPlatform->GetDevice(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_VulkanPlatform->GetDevice(), m_SwapChain, &imageCount, nullptr);
	m_SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_VulkanPlatform->GetDevice(), m_SwapChain, &imageCount, m_SwapChainImages.data());

	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent = extent;
}

void VulkanSwapChain::CreateImageViews()
{
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	for (size_t swapChainImagesIndex = 0; swapChainImagesIndex < m_SwapChainImages.size(); swapChainImagesIndex++)
	{
		m_SwapChainImageViews[swapChainImagesIndex] = CreateImageView(m_SwapChainImages[swapChainImagesIndex], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

void VulkanSwapChain::CreateColorResources()
{
	VkFormat colorFormat = m_SwapChainImageFormat;
	VkExtent2D swapChainExtent = GetSwapChainExtent();
	m_ColorImage.resize(ImageCount());
	m_ColorImageMemory.resize(ImageCount());
	m_ColorImageView.resize(ImageCount());
	for (int colorImagesIndex = 0; colorImagesIndex < m_ColorImage.size(); colorImagesIndex++)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = swapChainExtent.width;
		imageInfo.extent.height = swapChainExtent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = colorFormat;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		imageInfo.samples = GlobalFunctionLibrary::GetVulkanData()->MSAASamples;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		m_VulkanPlatform->CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_ColorImage[colorImagesIndex], m_ColorImageMemory[colorImagesIndex]);
		m_ColorImageView[colorImagesIndex] = CreateImageView(m_ColorImage[colorImagesIndex], colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

void VulkanSwapChain::CreateDepthResources()
{
	VkFormat depthFormat = FindDepthFormat();
	m_SwapChainDepthFormat = depthFormat;
	VkExtent2D swapChainExtent = GetSwapChainExtent();

	m_DepthImages.resize(ImageCount());
	m_DepthImageMemorys.resize(ImageCount());
	m_DepthImageViews.resize(ImageCount());

	for (int depthImagesIndex = 0; depthImagesIndex < m_DepthImages.size(); depthImagesIndex++) 
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = swapChainExtent.width;
		imageInfo.extent.height = swapChainExtent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = depthFormat;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imageInfo.samples = GlobalFunctionLibrary::GetVulkanData()->MSAASamples;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.flags = 0;

		m_VulkanPlatform->CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImages[depthImagesIndex], m_DepthImageMemorys[depthImagesIndex]);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_DepthImages[depthImagesIndex];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = depthFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_VulkanPlatform->GetDevice(), &viewInfo, nullptr, &m_DepthImageViews[depthImagesIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}
	}
}

void VulkanSwapChain::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = GetSwapChainImageFormat();
	colorAttachment.samples = GlobalFunctionLibrary::GetVulkanData()->MSAASamples;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = FindDepthFormat();
	depthAttachment.samples = GlobalFunctionLibrary::GetVulkanData()->MSAASamples;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachmentResolve{};
	colorAttachmentResolve.format = GetSwapChainImageFormat();
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentResolveRef{};
	colorAttachmentResolveRef.attachment = 2;
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	subpass.pResolveAttachments = &colorAttachmentResolveRef;

	VkSubpassDependency dependency = {};
	dependency.dstSubpass = 0;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.srcAccessMask = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

	std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(m_VulkanPlatform->GetDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void VulkanSwapChain::CreateFramebuffers()
{
	m_SwapChainFramebuffers.resize(ImageCount());
	for (size_t ImageIndex = 0; ImageIndex < ImageCount(); ImageIndex++) 
	{
		std::array<VkImageView, 3> attachments = { m_ColorImageView[ImageIndex], m_DepthImageViews[ImageIndex], m_SwapChainImageViews[ImageIndex] };

		VkExtent2D swapChainExtent = GetSwapChainExtent();
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_VulkanPlatform->GetDevice(), &framebufferInfo, nullptr, &m_SwapChainFramebuffers[ImageIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void VulkanSwapChain::CreateSyncObjects()
{
	m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	m_ImagesInFlight.resize(ImageCount(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t FrameInFlightIndex = 0; FrameInFlightIndex < MAX_FRAMES_IN_FLIGHT; FrameInFlightIndex++) 
	{
		if (vkCreateSemaphore(m_VulkanPlatform->GetDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[FrameInFlightIndex]) != VK_SUCCESS ||
			vkCreateSemaphore(m_VulkanPlatform->GetDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[FrameInFlightIndex]) != VK_SUCCESS ||
			vkCreateFence(m_VulkanPlatform->GetDevice(), &fenceInfo, nullptr, &m_InFlightFences[FrameInFlightIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _AvailableFormats)
{
	for (const auto& availableFormat : _AvailableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
		{
			return availableFormat;
		}
	}

	return _AvailableFormats[0];
}

VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _AvailablePresentModes)
{
	for (const auto& availablePresentMode : _AvailablePresentModes) 
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
		{
			std::cout << "Present mode: Mailbox" << std::endl;
			return availablePresentMode;
		}
	}

	std::cout << "Present mode: V-Sync" << std::endl;
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& _Capabilities)
{
	if (_Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return _Capabilities.currentExtent;
	}
	else 
	{
		VkExtent2D actualExtent = m_WindowExtent;
		actualExtent.width = std::max( _Capabilities.minImageExtent.width, std::min(_Capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(_Capabilities.minImageExtent.height, std::min(_Capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

float VulkanSwapChain::ExtentAspectRatio()
{
	return static_cast<float>(m_SwapChainExtent.width) / static_cast<float>(m_SwapChainExtent.height);
}

VkFormat VulkanSwapChain::FindDepthFormat()
{
	return m_VulkanPlatform->FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkResult VulkanSwapChain::AcquireNextImage(uint32_t* _ImageIndex)
{
	vkWaitForFences(m_VulkanPlatform->GetDevice(), 1, &m_InFlightFences[m_CurrentFrame],	VK_TRUE, std::numeric_limits<uint64_t>::max());

	VkResult result = vkAcquireNextImageKHR(m_VulkanPlatform->GetDevice(), m_SwapChain, std::numeric_limits<uint64_t>::max(), m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, _ImageIndex);

	return result;
}

VkResult VulkanSwapChain::SubmitCommandBuffers(const VkCommandBuffer* _Buffers, uint32_t* _ImageIndex)
{
	if (m_ImagesInFlight[*_ImageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(m_VulkanPlatform->GetDevice(), 1, &m_ImagesInFlight[*_ImageIndex], VK_TRUE, UINT64_MAX);
	}
	m_ImagesInFlight[*_ImageIndex] = m_InFlightFences[m_CurrentFrame];

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = _Buffers;

	VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(m_VulkanPlatform->GetDevice(), 1, &m_InFlightFences[m_CurrentFrame]);
	if (vkQueueSubmit(m_VulkanPlatform->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_SwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = _ImageIndex;

	auto result = vkQueuePresentKHR(m_VulkanPlatform->GetPresentQueue(), &presentInfo);

	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	return result;
}

bool VulkanSwapChain::CompareSwapFormats(const VulkanSwapChain& _SwapChain)
{
	return _SwapChain.m_SwapChainDepthFormat == m_SwapChainDepthFormat && _SwapChain.m_SwapChainImageFormat == m_SwapChainImageFormat;;
}

VkImageView VulkanSwapChain::CreateImageView(VkImage _Image, VkFormat _Format, VkImageAspectFlags _AspectFlags, uint32_t _MipLevels)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = _Image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = _Format;
	viewInfo.subresourceRange.aspectMask = _AspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = _MipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(m_VulkanPlatform->GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image view!");
	}

	return imageView;
}
