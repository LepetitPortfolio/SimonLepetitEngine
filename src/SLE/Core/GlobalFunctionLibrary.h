#pragma once
#define GLFW_INCLUDE_VULKAN
#include "../System/VulkanStructs.h"

class GlobalFunctionLibrary
{
public:

	static class Engine* GetEngine();

	static struct Config* GetConfig();

	static class WindowPlatform* GetWindowPlatform();

	static class GLFWwindow* GetWindow();

	static class VulkanDevice* GetVulkanDevice();
	
	static class VulkanPlatform* GetVulkanPlatform();

	static struct VulkanData* GetVulkanData();

	static class Scene* GetCurrentScene();

	static class AssetDataManager* GetAssetDataManager();

};