#pragma once
#define GLFW_INCLUDE_VULKAN
#include "../System/VulkanStructs.h"

class GlobalFunctionLibrary
{
public:

	static class Engine* GetEngine();

	static class WindowPlatform* GetWindowPlatform();

	static class GLFWwindow* GetWindow();

	static const VkDevice GetVulkanDevice();
	
	static class VulkanPlatform* GetVulkanPlatform();

	static class VulkanRenderer* GetVulkanRenderer();

	static const VulkanData* GetVulkanData();

	static class Scene* GetCurrentScene();

	static class AssetDataManager* GetAssetDataManager();

};