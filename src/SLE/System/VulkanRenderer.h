#pragma once
#include "VulkanPlatform.h"
#include "VulkanSwapChain.h"
#include "WindowPlatform.h"

#include <cassert>
#include <memory>
#include <vector>


class VulkanRenderer
{
public:
	VulkanRenderer();
	~VulkanRenderer();

	VulkanRenderer(const VulkanRenderer&) = delete;
	VulkanRenderer& operator=(const VulkanRenderer&) = delete;


	VulkanSwapChain* GetSwapChain() { return m_SwapChain.get(); }
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

	std::unique_ptr<VulkanSwapChain> m_SwapChain;
	std::vector<VkCommandBuffer> m_CommandBuffers;

	uint32_t m_CurrentImageIndex;
	int m_CurrentFrameIndex{ 0 };
	bool m_IsFrameStarted{ false };

	void CreateCommandBuffers();
	void FreeCommandBuffers();
	void RecreateSwapChain();
};
