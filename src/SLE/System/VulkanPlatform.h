#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderer.h"
#include "VulkanCommandManager.h"
#include "VulkanBufferManager.h"
//#include "TextureManager.h"
//#include "DescriptorManager.h"
//#include "GuiManager.h"

#include "VulkanStructs.h"

#include "../Core/Delegate/DelegateInclude.h"

#include <memory>
#include <iostream>
#include <vector>


class WindowPlatform;

class VulkanPlatform
{

public:

    VulkanPlatform();
    ~VulkanPlatform();

	VulkanDevice* GetDevice() { return m_VulkanDevice.get(); }
    VulkanRenderer* GetVulkanRenderer() { return m_VulkanRenderer.get(); }
    VulkanCommandManager* GetCommandManager() { return m_CommandManager.get(); }
	VulkanBufferManager* GetBufferManager() { return m_BufferManager.get(); }
	VulkanSwapchain* GetVulkanSwapchain() { return m_VulkanSwapchain.get(); }

    VkFramebuffer GetFrameBuffer(int _Index) { return m_SwapchainFramebuffers[_Index]; }

	VulkanData& GetVulkanData() { return m_VulkanData; }

    void InitVulkan(WindowPlatform* _WindowPlatform);
    void DrawFrame(class CameraBase* _Camera, float _DeltaTime);

    void WaitIdle();

    void Cleanup();
    void RecreateSwapchain();
    void CreateSyncObjects();
    void DestroySyncObjects();
    void CreateDepthResources();
    void CreateFramebuffers();

protected:

    std::unique_ptr<VulkanInstance> m_VulkanInstance;
    std::unique_ptr<VulkanDevice> m_VulkanDevice;
    std::unique_ptr<VulkanSwapchain> m_VulkanSwapchain;
    std::unique_ptr<VulkanRenderer> m_VulkanRenderer;
    std::unique_ptr<VulkanCommandManager> m_CommandManager;
    std::unique_ptr<VulkanBufferManager> m_BufferManager;
    //std::unique_ptr<TextureManager> m_TextureManager;
    //std::unique_ptr<DescriptorManager> m_DescriptorManager;
    //std::unique_ptr<GuiManager> m_GuiManager;

    std::vector<VkFramebuffer> m_SwapchainFramebuffers{};

    VulkanData m_VulkanData{};

    bool m_FramebufferResized = false;

    virtual void OnCleanup() {}

};
