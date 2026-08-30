#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <set>


struct QueueFamilyIndices;
struct SwapChainSupportDetails;

class VulkanInstance;

class VulkanDevice 
{
public:
    VulkanDevice();
    ~VulkanDevice();

    void Initialize(const VulkanInstance& _Instance, VkSurfaceKHR _Surface);
    void Cleanup();

    VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
    VkDevice GetLogicalDevice() const { return m_Device; }
    VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
    VkQueue GetPresentQueue() const { return m_PresentQueue; }

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice _Device) const;
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice _Device) const;
    uint32_t FindMemoryType(uint32_t _TypeFilter, VkMemoryPropertyFlags _Properties) const;

private:
    const VulkanInstance* m_VulkanInstance = nullptr;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;
    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkQueue m_PresentQueue = VK_NULL_HANDLE;

    const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    void PickPhysicalDevice();
    void CreateLogicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice _Device) const;
    bool CheckDeviceExtensionSupport(VkPhysicalDevice _Device) const;
};

