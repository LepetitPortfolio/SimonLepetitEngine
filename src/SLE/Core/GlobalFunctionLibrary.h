#pragma once
#define GLFW_INCLUDE_VULKAN
#include "../System/VulkanStructs.h"

class GlobalFunctionLibrary
{
public:

	static class Engine* GetEngine();

	static const VkDevice GetVulkanDevice();
	
	static class VulkanPlatform* GetVulkanPlatform();

	static const VulkanData* GetVulkanData();

	static class Scene* GetCurrentScene();

	static class AssetDataManager* GetAssetDataManager();

};