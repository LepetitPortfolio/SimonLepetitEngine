#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <string>
#include <vector>

class VulkanPlatform;
class VulkanDevice;

class VulkanSwapchain 
{
public:
    VulkanSwapchain();
    ~VulkanSwapchain();

    void Initialize(const VulkanDevice& _Device, VkSurfaceKHR _Surface, GLFWwindow* _Window);
    void Cleanup();
    void RecreateSwapchain(GLFWwindow* _Window);

    VkSwapchainKHR GetSwapchain() const { return m_Swapchain; }
    const std::vector<VkImage>& GetImages() const { return m_SwapchainImages; }
    VkFormat GetImageFormat() const { return m_SwapchainImageFormat; }
    VkExtent2D GetExtent() const { return m_SwapchainExtent; }
    const std::vector<VkImageView>& GetImageViews() const { return m_SwapchainImageViews; }
	uint32_t GetImageCount() const { return static_cast<uint32_t>(m_SwapchainImages.size()); }

private:
    const VulkanDevice* m_VulkanDevice = nullptr;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_SwapchainImages;
    VkFormat m_SwapchainImageFormat;
    VkExtent2D m_SwapchainExtent;
    std::vector<VkImageView> m_SwapchainImageViews;

    //std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    //std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    //std::vector<VkFence> m_InFlightFences;
    //std::vector<VkFence> m_ImagesInFlight;
    //size_t m_CurrentFrame = 0;

    void CreateSwapchain(GLFWwindow* _Window);
    void CreateImageViews();
    void CleanupSwapchain();

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _AvailableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _AvailablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& _Capabilities, GLFWwindow* _Window);
};