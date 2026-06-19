#pragma once
#include "VulkanPlatform.h"
#include "SwapChain.h"
#include "WindowPlatform.h"

#include <cassert>
#include <memory>
#include <vector>


class Renderer
{
public:
	Renderer(WindowPlatform& _Window, VulkanPlatform& _VulkanPlatform);
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	VkRenderPass GetSwapChainRenderPass() const { return m_SwapChain->GetRenderPass(); }
	float GetAspectRatio() const { return m_SwapChain->ExtentAspectRatio(); }
	bool IsFrameInProgress() const { return m_IsFrameStarted; }

	VkCommandBuffer GetCurrentCommandBuffer() const;
	int GetFrameIndex() const;

	VkCommandBuffer BeginFrame();
	void EndFrame();
	void BeginSwapChainRenderPass(VkCommandBuffer _CommandBuffer);
	void EndSwapChainRenderPass(VkCommandBuffer _CommandBuffer);

private:

	WindowPlatform& m_Window;
	VulkanPlatform& m_VulkanPlatform;
	std::unique_ptr<SwapChain> m_SwapChain;
	std::vector<VkCommandBuffer> m_CommandBuffers;

	uint32_t m_CurrentImageIndex;
	int m_CurrentFrameIndex{ 0 };
	bool m_IsFrameStarted{ false };

	void CreateCommandBuffers();
	void FreeCommandBuffers();
	void RecreateSwapChain();
};
