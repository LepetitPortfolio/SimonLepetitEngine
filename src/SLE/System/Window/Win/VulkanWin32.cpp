#include "../../../DataConfig.h"

#if PLATFORM_WINDOWS

#include "../VulkanPlatform.h"

#include <cstdint>
#include <string_view>
#include <vector>

#define VK_USE_PLATFORM_XLIB_KHR
#define VK_NO_PROTOTYPES
#include <vulkan.h>


struct VulkanLibraryWrapper_str
{
    void* Library{};
    PFN_vkGetInstanceProcAddr VkGetInstanceProcAddr{};
    PFN_vkEnumerateInstanceLayerProperties VkEnumerateInstanceLayerProperties{};
    PFN_vkEnumerateInstanceExtensionProperties VkEnumerateInstanceExtensionProperties{};

    
    ~VulkanLibraryWrapper_str()
    {
        if(Library)
        {
            dlclose(Library);
        }
    }

    bool LoadLibrary()
    {
        if(Library)
        {
            return true;
        }

        Library = dlopen("libvulkan.so.1", RTLD_LAZY);

        if(!Library)
        {
            return false;
        }

        if(!LoadEntryPoint(VkGetInstanceProcAddr, "vkGetInstanceProcAddr"))
        {
            dlclose(Library);
            Library = nullptr;
            return false;
        }

        if(!LoadEntryPoint(VkEnumerateInstanceLayerProperties, "vkEnumerateInstanceLayerProperties"))
        {
            dlclose(Library);
            Library = nullptr;
            return false;
        }

        if(!LoadEntryPoint(VkEnumerateInstanceExtensionProperties, "vkEnumerateInstanceExtensionProperties"))
        {
            dlclose(Library);
            Library = nullptr;
            return false;
        }

        return true;
    }

    template<typename T>
    bool LoadEntryPoint(T& _EntryPoint, const char* _Name)
    {
        _EntryPoint = reinterpret_cast<T>(dlsym(Library, _Name));

        return _EntryPoint != nullptr;
    }

};

VulkanLibraryWrapper_str VkLibraryWrapper;

bool VulkanPlatformIsAvailable(bool _RequireGraphics = true)
{
    static bool checked  = false;
    static bool computeAvailable  = false;
    static bool graphicsAvailable  = false;

    if(!checked)
    {
        checked = true;

        computeAvailable = VkLibraryWrapper.LoadLibrary();

        graphicsAvailable = computeAvailable;

        if(graphicsAvailable)
        {
            std::vector<VkExtensionProperties> extensionProperties;
            std::uint32_t extensionCount = 0;
            
            VkLibraryWrapper.VkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
            
            extensionProperties.resize(extensionCount);

            VkLibraryWrapper.VkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

            bool hasVkKhrSurface = false;
            bool hasVkKhrPlatformSurface = false;

            for(const VkExtensionProperties& extension : extensionProperties)
            {
                if(std::string_view(extension.extensionName) == VK_KHR_SURFACE_EXTENSION_NAME)
                {
                    hasVkKhrSurface = true;
                }
                else if(std::string_view(extension.extensionName) == VK_KHR_XLIB_SURFACE_EXTENSION_NAME)
                {
                    hasVkKhrPlatformSurface = true;
                }
            }

            if(!hasVkKhrSurface || !hasVkKhrPlatformSurface)
            {
                graphicsAvailable = false;
            }
        }
    }

    if(_RequireGraphics)
    {
        return graphicsAvailable;
    }

    return computeAvailable;

}

VulkanFunctionPointer GetVulkanPlatformFunction(const char* _Name)
{
    if(!IsAvailable(false))
    {
        return nullptr;
    }

    return reinterpret_cast<VulkanFunctionPointer>(dlsym(VkLibraryWrapper.Library, _Name));
}

const std::vector<const char*>& GetVulkanPlatformGraphicsRequiredInstanceExtensions()
{
    static const std::vector<const char*> extensions {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XLIB_SURFACE_EXTENSION_NAME};
    return extensions;
}

bool CreateVulkanPlatformSurface(const VkInstance& _Instance, WindowHandle _WindowHandle, VkSurfaceKHR _Surface, const VkAllocationCallbacks* _Allocator)
{
    if(!IsAvailable())
    {
        return false;
    }

    VkInstance instance = _Instance;

    auto vkCreateXlibSurfaceKHR = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(VkLibraryWrapper.VkGetInstanceProcAddr(instance, "vkCreateXlibSurfaceKHR"));

    if(!vkCreateXlibSurfaceKHR)
    {
        return false;
    }

    const auto display = OpenDisplay();
    VkXlibSurfaceCreateInfoKHR vkSurfaceCreateInfo = VkXlibSurfaceCreateInfoKHR{};
    vkSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    vkSurfaceCreateInfo.dpy = display.get();
    vkSurfaceCreateInfo.Window_cls = _WindowHandle;

    const bool result = (vkCreateXlibSurfaceKHR(_Instance, &vkSurfaceCreateInfo, _Allocator, &_Surface) == VK_SUCCESS);

    return result;
}

#endif