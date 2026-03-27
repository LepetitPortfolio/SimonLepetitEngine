#pragma once
#include "Vulkan.h"

#include <vector>


bool VulkanPlatformIsAvailable(bool _RequireGraphics = true);

VulkanFunctionPointer GetVulkanPlatformFunction(const char* _Name);

const std::vector<const char*>& GetVulkanPlatformGraphicsRequiredInstanceExtensions();

bool CreateVulkanPlatformSurface(const VkInstance& _Instance, WindowHandle _WindowHandle, VkSurfaceKHR _Surface, const VkAllocationCallbacks* _Allocator);
