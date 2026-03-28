#pragma once
#include "PlatformConfig.h"

#if defined(PLATFORM_WINDOWS)

    #include<string>

    inline std::string GetErrorString(DWORD _Error)
    {
        PTCHAR buffer = nullptr;

        DWORD flags = FORMAT_MESSAGE_MAX_WIDTH_MASK | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;

        if (FormatMessage(flags, nullptr, _Error, 0, reinterpret_cast<PTCHAR>(&buffer), 0, nullptr) == 0)
        {
            return "Unknown error !";
        }

        const String message = buffer;
        LocalFree(buffer);
        return message.ToAnsiString();
    }

#elif defined(PLATFORM_LINUX)

    template <typename T>
    struct XDeleter
    {
        void operator()(T* _Data) const
        {
            XFree(_Data);
        }
    };

    template <typename T>
    using X11Ptr = std::unique_ptr<T, XDeleter<std::remove_all_extents_t<T>>>;

#else

#endif


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
