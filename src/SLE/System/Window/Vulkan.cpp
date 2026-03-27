#include "Vulkan.h"

#include "VulkanPlatform.h"

#include <cassert>

bool IsAvailable(bool _RequireGraphics = true)
{
#if defined(VULKAN_IMPLEMENTATION_NOT_AVAILABLE)

    return false;

#else

    return VulkanPlatformIsAvailable(_RequireGraphics);

#endif
}
    
VulkanFunctionPointer GetFunction(const char* _Name)
{

    assert(_Name && "Name cannot be a null pointer");

#if defined(VULKAN_IMPLEMENTATION_NOT_AVAILABLE)

    return nullptr;

#else

    return GetVulkanPlatformFunction(_Name);

#endif
}

const std::vector<const char*>& GetGraphicsRequiredInstanceExtensions()
{
#if defined(VULKAN_IMPLEMENTATION_NOT_AVAILABLE)

    static const std::ector<const char*> empty;

    return empty;

#else

    return GetVulkanPlatformGraphicsRequiredInstanceExtensions();

#endif
}