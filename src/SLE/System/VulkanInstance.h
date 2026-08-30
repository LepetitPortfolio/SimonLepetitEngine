#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class VulkanInstance 
{
public:
	VulkanInstance();
	~VulkanInstance();

	VulkanInstance(const VulkanInstance&) = delete;
	VulkanInstance& operator=(const VulkanInstance&) = delete;

	void Initialize();
	void Cleanup();

	VkInstance GetInstance() const { return m_Instance; }
	bool IsValidationEnabled() const { return m_EnableValidationLayers; }
	const std::vector<const char*>& GetValidationLayers() const { return m_ValidationLayers; }
private:
	VkInstance m_Instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
	bool m_EnableValidationLayers;

	const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	void CreateInstance();
	void SetupDebugMessenger();
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& _CreateInfo);
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT _MssageSeverity, VkDebugUtilsMessageTypeFlagsEXT _MessageType, const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData, void* _pUserData);

};

VkResult CreateDebugUtilsMessengerEXT(VkInstance _Instance, const VkDebugUtilsMessengerCreateInfoEXT* _pCreateInfo,	const VkAllocationCallbacks* _pAllocator, VkDebugUtilsMessengerEXT* _pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance _Instance, VkDebugUtilsMessengerEXT _DebugMessenger, const VkAllocationCallbacks* _pAllocator);