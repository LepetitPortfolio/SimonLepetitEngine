#include "VulkanDevice.h"
#include "VulkanInstance.h"

#include "VulkanStructs.h"

#include <iostream>
#include <stdexcept>
#include <set>

VulkanDevice::VulkanDevice() 
{
}

VulkanDevice::~VulkanDevice() 
{
    Cleanup();
}

void VulkanDevice::Initialize(const VulkanInstance& _Instance, VkSurfaceKHR _Surface) 
{
    this->m_VulkanInstance = &_Instance;
    this->m_Surface = _Surface;

    PickPhysicalDevice();
    CreateLogicalDevice();
}

void VulkanDevice::Cleanup() 
{
    if (m_Device != VK_NULL_HANDLE) 
    {
        vkDestroyDevice(m_Device, nullptr);
        m_Device = VK_NULL_HANDLE;
    }
}

void VulkanDevice::PickPhysicalDevice() 
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_VulkanInstance->GetInstance(), &deviceCount, nullptr);

    if (deviceCount == 0) 
    {
        std::cerr << "failed to find physical device" << std::endl;
        throw std::runtime_error("failed to find physical device");
    }
    else 
    {
        std::cout << "no. of GPUs - " << deviceCount << std::endl;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_VulkanInstance->GetInstance(), &deviceCount, devices.data());

    for (const auto& device : devices) 
    {
        if (IsDeviceSuitable(device)) 
        {
            m_PhysicalDevice = device;
            break;
        }
    }

    if (m_PhysicalDevice == VK_NULL_HANDLE) 
    {
        std::cerr << "failed to find suitable GPU" << std::endl;
        throw std::runtime_error("failed to find suitable GPU");
    }
    else {
        std::cout << "Suitable gpu found - " << m_PhysicalDevice << std::endl;
    }
}

void VulkanDevice::CreateLogicalDevice() 
{
    QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };
    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies) 
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

    if (m_VulkanInstance->IsValidationEnabled()) 
    {
        const auto& validationLayers = m_VulkanInstance->GetValidationLayers();
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else 
    {
        createInfo.enabledLayerCount = 0;
    }

    VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);
    if (result != VK_SUCCESS) 
    {
        std::cout << "Failed to create logical device - " << result << std::endl;
        throw std::runtime_error("Failed to create logical device");
    }
    else 
    {
        std::cout << "Successfully created logical device - " << result << std::endl;
    }

    vkGetDeviceQueue(m_Device, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, indices.PresentFamily.value(), 0, &m_PresentQueue);
}

bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice _Device) const 
{
    QueueFamilyIndices indices = FindQueueFamilies(_Device);
    bool extensionsSupported = CheckDeviceExtensionSupport(_Device);
    std::cout << "Extensions are supported by the device - " << extensionsSupported << std::endl;
    bool swapChainAdequate = false;
    if (extensionsSupported) 
    {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_Device);
        swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
        std::cout << "Swapchain adequate for the device - " << swapChainAdequate << std::endl;
    }
    return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}

bool VulkanDevice::CheckDeviceExtensionSupport(VkPhysicalDevice _Device) const 
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(_Device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(_Device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

    for (const auto& extension : availableExtensions) 
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice _Device) const 
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_Device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_Device, &queueFamilyCount, queueFamilies.data());

    int queueFamilyIndex = 0;
    bool bBreak = false;

    while ((!bBreak) && (queueFamilyIndex < queueFamilies.size()))
    {
        if (queueFamilies[queueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.GraphicsFamily = queueFamilyIndex;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(_Device, queueFamilyIndex, m_Surface, &presentSupport);
        if (presentSupport)
        {
            indices.PresentFamily = queueFamilyIndex;
        }

        if (indices.IsComplete())
        {
            bBreak = true;
        }

        queueFamilyIndex++;
    }


    return indices;
}

SwapChainSupportDetails VulkanDevice::QuerySwapChainSupport(VkPhysicalDevice _Device) const 
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_Device, m_Surface, &details.Capabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(_Device, m_Surface, &formatCount, nullptr);
    if (formatCount != 0) 
    {
        details.Formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(_Device, m_Surface, &formatCount, details.Formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(_Device, m_Surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) 
    {
        details.PresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(_Device, m_Surface, &presentModeCount, details.PresentModes.data());
    }

    return details;
}

uint32_t VulkanDevice::FindMemoryType(uint32_t _TypeFilter, VkMemoryPropertyFlags _Properties) const 
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
    {
        if ((_TypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & _Properties) == _Properties) 
        {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}


