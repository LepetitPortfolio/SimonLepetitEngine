#include "VulkanRenderer.h"

#include "WindowPlatform.h"
#include "VulkanPlatform.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

#include "../Common/Vertex.h"
#include "../Core/GlobalFunctionLibrary.h"

#include "AssetDataManager.h"
#include "AssetDataManagerBase.h"
#include "../Graphics/Shader.h"

#include <iostream>
#include <stdexcept>
#include <fstream>
#include <array>
#include <cassert>
#include <glm/glm.hpp>

VulkanRenderer::VulkanRenderer()
{
}

VulkanRenderer::~VulkanRenderer() 
{ 
	Cleanup();
}

void VulkanRenderer::Initialize(VulkanDevice& _Device, VulkanSwapchain& _Swapchain)
{
	m_VulkanDevice = &_Device;
	m_VulkanSwapchain = &_Swapchain;

	CreateRenderPass(m_VulkanSwapchain->GetImageFormat());
}

void VulkanRenderer::RecreatePipelines()
{
	AssetDataManagerBase* shaderManager = GlobalFunctionLibrary::GetAssetDataManager()->GetShaderManager();
	auto& shaderDataList = *shaderManager->GetData();
	for (auto& data : shaderDataList)
	{
		Shader* shader = static_cast<Shader*>(data.second);
		if (shader)
		{
			shader->RecreatePipeline();
		}
	}
}

void VulkanRenderer::Cleanup() 
{
	if (m_VulkanDevice) 
	{
		vkDestroyRenderPass(m_VulkanDevice->GetLogicalDevice(), m_RenderPass, nullptr);
	}
}

VkCommandBuffer VulkanRenderer::GetCurrentCommandBuffer() const
{
	assert(m_IsFrameStarted && "Cannot get command buffer when frame not in progress");
	return m_CommandBuffers[m_CurrentFrameIndex];
}

float VulkanRenderer::GetAspectRatio() const
{
	if (m_VulkanSwapchain)
	{
		auto extent = m_VulkanSwapchain->GetExtent();
		return static_cast<float>(extent.width) / static_cast<float>(extent.height);
	}
	return 0.0f;
}

bool VulkanRenderer::HasStencilComponent(VkFormat _Format)
{
	return _Format == VK_FORMAT_D32_SFLOAT_S8_UINT || _Format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat VulkanRenderer::FindDepthFormat()
{
	return FindSupportedFormat
	(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

VkFormat VulkanRenderer::FindSupportedFormat(const std::vector<VkFormat>& _Candidates, VkImageTiling _Tiling, VkFormatFeatureFlags _Features)
{
	for (VkFormat format : _Candidates) 
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_VulkanDevice->GetPhysicalDevice(), format, &props);

		if (_Tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & _Features) == _Features)
		{
			return format;
		}
		else if (_Tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & _Features) == _Features)
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

void VulkanRenderer::CreateRenderPass(VkFormat _SwapChainImageFormat)
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = _SwapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = FindDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkResult result = vkCreateRenderPass(m_VulkanDevice->GetLogicalDevice(), &renderPassInfo, nullptr, &m_RenderPass);
	if (result != VK_SUCCESS) 
	{
		std::cout << "failed to create render pass - " << result << std::endl;
		throw std::runtime_error("failed to create render pass!");
	}
	else 
	{
		std::cout << "successfully created render pass - " << result << std::endl;
	}
}
