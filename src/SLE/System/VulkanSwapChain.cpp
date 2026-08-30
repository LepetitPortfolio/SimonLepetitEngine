#include "VulkanSwapchain.h"

#include "VulkanPlatform.h"
#include "VulkanDevice.h"

#include "../Core/GlobalFunctionLibrary.h"
#include "../Common/Error.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <limits>

VulkanSwapchain::VulkanSwapchain() 
{
}

VulkanSwapchain::~VulkanSwapchain() 
{
    Cleanup();
}

void VulkanSwapchain::Initialize(const VulkanDevice& device, VkSurfaceKHR surface, GLFWwindow* _Window) 
{
    this->m_VulkanDevice = &device;
    this->m_Surface = surface;

    CreateSwapchain(_Window);
    CreateImageViews();
}

void VulkanSwapchain::Cleanup() 
{
    CleanupSwapchain();
}

void VulkanSwapchain::RecreateSwapchain(GLFWwindow* _Window)
{
    CleanupSwapchain();
    CreateSwapchain(_Window);
    CreateImageViews();
}

void VulkanSwapchain::CreateSwapchain(GLFWwindow* _Window) 
{
    SwapChainSupportDetails swapChainSupport = m_VulkanDevice->QuerySwapChainSupport(m_VulkanDevice->GetPhysicalDevice());

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.Capabilities, _Window);

    uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
    if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount) 
    {
        imageCount = swapChainSupport.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_Surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = m_VulkanDevice->FindQueueFamilies(m_VulkanDevice->GetPhysicalDevice());
    uint32_t queueFamilyIndices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };
    if (indices.GraphicsFamily != indices.PresentFamily) 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(m_VulkanDevice->GetLogicalDevice(), &createInfo, nullptr, &m_Swapchain);

    if (result != VK_SUCCESS) 
    {
        std::cout << "Failed to create swapchain - " << result << std::endl;
        throw std::runtime_error("failed to create swap chain!");
    }
    else {
        std::cout << "Swapchain created successfully" << std::endl;
    }

    vkGetSwapchainImagesKHR(m_VulkanDevice->GetLogicalDevice(), m_Swapchain, &imageCount, nullptr);
    m_SwapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_VulkanDevice->GetLogicalDevice(), m_Swapchain, &imageCount, m_SwapchainImages.data());
    
    m_SwapchainImageFormat = surfaceFormat.format;
    m_SwapchainExtent = extent;
}

void VulkanSwapchain::CreateImageViews() 
{
    m_SwapchainImageViews.resize(m_SwapchainImages.size());
    for (uint32_t i = 0; i < m_SwapchainImages.size(); i++) 
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_SwapchainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_SwapchainImageFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_VulkanDevice->GetLogicalDevice(), &viewInfo, nullptr, &m_SwapchainImageViews[i]) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create image view!");
        }
    }
}

void VulkanSwapchain::CleanupSwapchain() 
{
    for (auto imageView : m_SwapchainImageViews) 
    {
        vkDestroyImageView(m_VulkanDevice->GetLogicalDevice(), imageView, nullptr);
    }
    m_SwapchainImageViews.clear();

    if (m_Swapchain != VK_NULL_HANDLE) 
    {
        vkDestroySwapchainKHR(m_VulkanDevice->GetLogicalDevice(), m_Swapchain, nullptr);
        m_Swapchain = VK_NULL_HANDLE;
    }
}

VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
{
    for (const auto& availableFormat : availableFormats) 
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) 
{
    for (const auto& availablePresentMode : availablePresentModes) 
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* _Window) 
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
    {
        return capabilities.currentExtent;
    }
    else 
    {
        int width, height;
        glfwGetFramebufferSize(_Window, &width, &height);

        VkExtent2D actualExtent = 
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}