#include "VulkanRenderer.h"
#include "../Core/GlobalFunctionLibrary.h"

#include <array>
#include <cassert>
#include <stdexcept>

VulkanRenderer::VulkanRenderer()
{
	RecreateSwapChain();
	CreateCommandBuffers();
}

VulkanRenderer::~VulkanRenderer() 
{ 
	FreeCommandBuffers(); 
}

VkCommandBuffer VulkanRenderer::GetCurrentCommandBuffer() const
{
	assert(m_IsFrameStarted && "Cannot get command buffer when frame not in progress");
	return m_CommandBuffers[m_CurrentFrameIndex];
}

int VulkanRenderer::GetFrameIndex() const
{
	assert(m_IsFrameStarted && "Cannot get frame index when frame not in progress");
	return m_CurrentFrameIndex;
}

VkCommandBuffer VulkanRenderer::BeginFrame()
{
	assert(!m_IsFrameStarted && "Can't call beginFrame while already in progress");

	auto result = m_SwapChain->AcquireNextImage(&m_CurrentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) 
	{
		RecreateSwapChain();
		return nullptr;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	m_IsFrameStarted = true;

	auto commandBuffer = GetCurrentCommandBuffer();
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	return commandBuffer;
}

void VulkanRenderer::EndFrame()
{
	assert(m_IsFrameStarted && "Can't call endFrame while frame is not in progress");
	auto commandBuffer = GetCurrentCommandBuffer();

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to record command buffer!");
	}

	auto result = m_SwapChain->SubmitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || GlobalFunctionLibrary::GetWindowPlatform()->WasWindowResized())
	{
		GlobalFunctionLibrary::GetWindowPlatform()->ResetWindowResizedFlag();
		RecreateSwapChain();
	}
	else if (result != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	m_IsFrameStarted = false;
	m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % VulkanSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void VulkanRenderer::BeginSwapChainRenderPass(VkCommandBuffer _CommandBuffer)
{
	assert(m_IsFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
	assert(_CommandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_SwapChain->GetRenderPass();
	renderPassInfo.framebuffer = m_SwapChain->GetFrameBuffer(m_CurrentImageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_SwapChain->GetSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_SwapChain->GetSwapChainExtent().width);
	viewport.height = static_cast<float>(m_SwapChain->GetSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0, 0}, m_SwapChain->GetSwapChainExtent() };
	vkCmdSetViewport(_CommandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(_CommandBuffer, 0, 1, &scissor);
}

void VulkanRenderer::EndSwapChainRenderPass(VkCommandBuffer _CommandBuffer)
{
	assert(m_IsFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
	assert(_CommandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
	vkCmdEndRenderPass(_CommandBuffer);
}

void VulkanRenderer::CreateCommandBuffers()
{
	m_CommandBuffers.resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = GlobalFunctionLibrary::GetVulkanPlatform()->GetCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	if (vkAllocateCommandBuffers(GlobalFunctionLibrary::GetVulkanDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void VulkanRenderer::FreeCommandBuffers()
{
	vkFreeCommandBuffers(GlobalFunctionLibrary::GetVulkanDevice(), GlobalFunctionLibrary::GetVulkanPlatform()->GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
	m_CommandBuffers.clear();
}

void VulkanRenderer::RecreateSwapChain()
{
	auto extent = GlobalFunctionLibrary::GetWindowPlatform()->GetExtent();
	while (extent.width == 0 || extent.height == 0) 
	{
		extent = GlobalFunctionLibrary::GetWindowPlatform()->GetExtent();
		glfwWaitEvents();
	}
	vkDeviceWaitIdle(GlobalFunctionLibrary::GetVulkanDevice());

	if (m_SwapChain == nullptr) 
	{
		m_SwapChain = std::make_unique<VulkanSwapChain>(extent);
	}
	else 
	{
		std::shared_ptr<VulkanSwapChain> oldSwapChain = std::move(m_SwapChain);
		m_SwapChain = std::make_unique<VulkanSwapChain>(extent, oldSwapChain);

		if (!oldSwapChain->CompareSwapFormats(*m_SwapChain.get())) 
		{
			throw std::runtime_error("Swap chain image(or depth) format has changed!");
		}
	}
}
