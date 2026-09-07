#include "VulkanPlatform.h"
#include "WindowPlatform.h"

#include "../GameplayConcepts/Scene.h"

#include "../Core/GlobalFunctionLibrary.h"
#include "../Common/Error.h"

#include <set>
#include <unordered_set>


VulkanPlatform::VulkanPlatform()
{
}

VulkanPlatform::~VulkanPlatform()
{
	Cleanup();
}

void VulkanPlatform::InitVulkan(WindowPlatform* _WindowPlatform)
{ 
    m_VulkanInstance = std::make_unique<VulkanInstance>();
    m_VulkanInstance->Initialize();

	_WindowPlatform->CreateWindowSurface(m_VulkanInstance->GetInstance(), &m_VulkanData.Surface);

    m_VulkanDevice = std::make_unique<VulkanDevice>();
    m_VulkanDevice->Initialize(*m_VulkanInstance, m_VulkanData.Surface);

    m_VulkanSwapchain = std::make_unique<VulkanSwapchain>();
    m_VulkanSwapchain->Initialize(*m_VulkanDevice, m_VulkanData.Surface, _WindowPlatform->GetWindow());

	m_VulkanRenderer = std::make_unique<VulkanRenderer>();
	m_VulkanRenderer->Initialize(*m_VulkanDevice, *m_VulkanSwapchain);

    m_CommandManager = std::make_unique<VulkanCommandManager>();
    m_CommandManager->Initialize(*m_VulkanDevice, GlobalFunctionLibrary::GetConfig()->MaxFramesInFlight);

    m_BufferManager = std::make_unique<VulkanBufferManager>();
    m_BufferManager->Initialize(*m_VulkanDevice, *m_CommandManager);

    //m_TextureManager = std::make_unique<TextureManager>();
    //m_TextureManager->Initialize(*m_VulkanDevice, *m_CommandManager, *m_BufferManager);

    //m_DescriptorManager = std::make_unique<DescriptorManager>();
    //m_DescriptorManager->Initialize(*m_VulkanDevice);

    CreateSyncObjects();
    CreateDepthResources();
    CreateFramebuffers();
}

void VulkanPlatform::DrawFrame(CameraBase* _Camera, float _DeltaTime)
{
    const uint32_t frameInFlight = m_VulkanData.CurrentFrameIndexInFlight;
    VkDevice device = m_VulkanDevice->GetLogicalDevice();

    VkFence inFlightFence = m_VulkanData.InFlightFences[frameInFlight];
    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

    VkSemaphore imageAvailableSemaphore = m_VulkanData.ImageAvailableSemaphores[frameInFlight];

    VulkanFrameInfo frameInfo{};
    frameInfo.FrameIndex = 0;
    frameInfo.CurrentFrameIndexInFlight = frameInFlight;
    frameInfo.Camera = _Camera;
    frameInfo.FrameTime = _DeltaTime;
    frameInfo.CommandBuffer = m_CommandManager->GetCommandBuffer(frameInFlight);

    VkResult acquireNextImageResult = vkAcquireNextImageKHR( device, m_VulkanSwapchain->GetSwapchain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &frameInfo.FrameIndex);

    if (acquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchain();
        return;
    }

    if (acquireNextImageResult != VK_SUCCESS && acquireNextImageResult != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // The command buffer is owned by the current frame-in-flight and is now
    // safe to reset because its fence has completed.
    m_CommandManager->ResetCommandBuffer(frameInFlight);

    vkResetFences(device, 1, &inFlightFence);

    // Begin the command buffer/render pass exactly once per frame.
    m_CommandManager->BeginCommandBuffer(frameInfo.CommandBuffer);
    m_CommandManager->BeginRenderPass( frameInfo.CommandBuffer, m_VulkanRenderer->GetRenderPass(), m_SwapchainFramebuffers[frameInfo.FrameIndex], m_VulkanSwapchain->GetExtent());

    GlobalFunctionLibrary::GetCurrentScene()->UpdateDraw(frameInfo);

    m_CommandManager->EndRenderPass(frameInfo.CommandBuffer);
    m_CommandManager->EndCommandBuffer(frameInfo.CommandBuffer);

    VkSemaphore renderFinishedSemaphore = m_VulkanData.RenderFinishedSemaphores[frameInfo.FrameIndex];

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &frameInfo.CommandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

    VkResult queueSubmitResult = vkQueueSubmit( m_VulkanDevice->GetGraphicsQueue(), 1, &submitInfo, inFlightFence);

    if (queueSubmitResult != VK_SUCCESS)
    {
        std::cout << "failed to submit draw command buffer - " << queueSubmitResult << std::endl;
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkSwapchainKHR swapChain = m_VulkanSwapchain->GetSwapchain();

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapChain;
    presentInfo.pImageIndices = &frameInfo.FrameIndex;

    VkResult queuePresentResult =
        vkQueuePresentKHR(m_VulkanDevice->GetPresentQueue(), &presentInfo);

    if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR || m_FramebufferResized)
    {
        m_FramebufferResized = false;
        RecreateSwapchain();
    }
    else if (queuePresentResult != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_VulkanData.CurrentFrameIndexInFlight = (frameInFlight + 1) % GlobalFunctionLibrary::GetConfig()->MaxFramesInFlight;
}

void VulkanPlatform::WaitIdle()
{
    if (m_VulkanDevice && m_VulkanDevice->GetLogicalDevice())
    {
        vkDeviceWaitIdle(m_VulkanDevice->GetLogicalDevice());
    }
}

void VulkanPlatform::Cleanup()
{
    if (!m_VulkanDevice || !m_VulkanDevice->GetLogicalDevice())
    {
        return;
    }

    WaitIdle();

    for (VkFramebuffer framebuffer : m_SwapchainFramebuffers)
    {
        if (framebuffer != VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(m_VulkanDevice->GetLogicalDevice(), framebuffer, nullptr);
        }
    }
    m_SwapchainFramebuffers.clear();

    OnCleanup();

    DestroySyncObjects();

    if (m_VulkanSwapchain)
    {
        m_VulkanSwapchain.reset();
    }
}

void VulkanPlatform::RecreateSwapchain()
{
    GLFWwindow* window = GlobalFunctionLibrary::GetWindow();
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    WaitIdle();

    for (VkFramebuffer framebuffer : m_SwapchainFramebuffers)
    {
        if (framebuffer != VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(m_VulkanDevice->GetLogicalDevice(), framebuffer, nullptr);
        }
    }
    m_SwapchainFramebuffers.clear();

    DestroySyncObjects();

    // Keep the TextureVoid object alive (it is owned by AssetDataManager),
    // but destroy and recreate its Vulkan resources so the depth image matches
    // the new swapchain extent.
    if (!m_VulkanData.DepthTextures.empty())
    {
        for (TextureVoid* depthTexture : m_VulkanData.DepthTextures)
        {
            if (depthTexture)
            {
                depthTexture->CleanupTextureVoid();
            }
        }

		m_VulkanData.DepthTextures.clear();
    }

    m_VulkanSwapchain->RecreateSwapchain(window);

    CreateSyncObjects();
    CreateDepthResources();
    CreateFramebuffers();
}

void VulkanPlatform::CreateSyncObjects()
{
    const uint32_t maxFramesInFlight =
        GlobalFunctionLibrary::GetConfig()->MaxFramesInFlight;
    const uint32_t swapchainImageCount =
        static_cast<uint32_t>(m_VulkanSwapchain->GetImages().size());

    m_VulkanData.ImageAvailableSemaphores.resize(maxFramesInFlight, VK_NULL_HANDLE);
    m_VulkanData.RenderFinishedSemaphores.resize(swapchainImageCount, VK_NULL_HANDLE);
    m_VulkanData.InFlightFences.resize(maxFramesInFlight, VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < maxFramesInFlight; ++i)
    {
        if (vkCreateSemaphore(
                m_VulkanDevice->GetLogicalDevice(),
                &semaphoreInfo,
                nullptr,
                &m_VulkanData.ImageAvailableSemaphores[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image available semaphore!");
        }

        if (vkCreateFence(
                m_VulkanDevice->GetLogicalDevice(),
                &fenceInfo,
                nullptr,
                &m_VulkanData.InFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create in-flight fence!");
        }
    }

    // A present semaphore is associated with a swapchain image, not with a
    // frame-in-flight. This avoids re-signaling a binary semaphore while the
    // presentation engine may still be using it.
    for (uint32_t i = 0; i < swapchainImageCount; ++i)
    {
        if (vkCreateSemaphore(
                m_VulkanDevice->GetLogicalDevice(),
                &semaphoreInfo,
                nullptr,
                &m_VulkanData.RenderFinishedSemaphores[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render finished semaphore!");
        }
    }
}

void VulkanPlatform::DestroySyncObjects()
{
    if (!m_VulkanDevice || !m_VulkanDevice->GetLogicalDevice())
    {
        return;
    }

    VkDevice device = m_VulkanDevice->GetLogicalDevice();

    for (VkSemaphore& semaphore : m_VulkanData.ImageAvailableSemaphores)
    {
        if (semaphore != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(device, semaphore, nullptr);
            semaphore = VK_NULL_HANDLE;
        }
    }

    for (VkSemaphore& semaphore : m_VulkanData.RenderFinishedSemaphores)
    {
        if (semaphore != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(device, semaphore, nullptr);
            semaphore = VK_NULL_HANDLE;
        }
    }

    for (VkFence& fence : m_VulkanData.InFlightFences)
    {
        if (fence != VK_NULL_HANDLE)
        {
            vkDestroyFence(device, fence, nullptr);
            fence = VK_NULL_HANDLE;
        }
    }

    m_VulkanData.ImageAvailableSemaphores.clear();
    m_VulkanData.RenderFinishedSemaphores.clear();
    m_VulkanData.InFlightFences.clear();
}

void VulkanPlatform::CreateDepthResources()
{
    if (m_VulkanData.DepthTextures.empty())
    {
        const size_t swapchainImageCount = m_VulkanSwapchain->GetImages().size();
        m_VulkanData.DepthTextures.resize(swapchainImageCount);

        for (size_t i = 0; i < swapchainImageCount; i++)
        {
            if (m_VulkanData.DepthTextures[i] == nullptr)
            {
                m_VulkanData.DepthTextures[i] = new TextureVoid();
            }
            m_VulkanData.DepthTextures[i]->GenerateDepthResources();
        }
    }

}

void VulkanPlatform::CreateFramebuffers()
{
    const std::vector<VkImageView>& swapChainImageViews = m_VulkanSwapchain->GetImageViews();
    m_SwapchainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) 
    {
        std::array<VkImageView, 2> attachments = 
        {
            swapChainImageViews[i],
            m_VulkanData.DepthTextures[i]->GetTextureImageView()
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_VulkanRenderer->GetRenderPass();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_VulkanSwapchain->GetExtent().width;
        framebufferInfo.height = m_VulkanSwapchain->GetExtent().height;
        framebufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(m_VulkanDevice->GetLogicalDevice(), &framebufferInfo, nullptr, &m_SwapchainFramebuffers[i]);
        if (result != VK_SUCCESS) 
        {
            std::cout << "failed to create framebuffer - " << i << " - " << result << std::endl;
            throw std::runtime_error("failed to create framebuffer!");
        }
        else 
        {
            std::cout << "successfully created framebuffer - " << i << " - " << result << std::endl;
        }
    }
}
