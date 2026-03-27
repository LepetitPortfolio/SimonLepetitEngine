#pragma once
#include "PlatformConfig.h"

#if defined(__LP64__) || defined(_Win64) || (defined(__x86_64__) && !defined(__ILP32__)) || defined(_M_X64) || \
    defined(__ia64) || defined(_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)

    using VkSurfaceKHR = struct VkSurfaceKHR_t*;

#else

    #include <cstdint>

    using VkSurfaceKHR =  std::uint64_t;

#endif

#if defined(PLATFORM_WINDOWS)

    #include "Window_cls/VulkanPlatform.h"

#elif defined(PLATFORM_LINUX)

    #include "Window/VulkanPlatform.h"

#else

    #define VULKAN_IMPLEMENTATION_NOT_AVAILABLE
    #error "Unsupported Vulkan on this platform"

#endif
