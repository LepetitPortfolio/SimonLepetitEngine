#include "GlobalFunctionLibrary.h"

#include "../Common/Error.h"
#include "../System/WindowPlatform.h"
#include "../System/VulkanIncludes.h"
#include "../System/VulkanStructs.h"
#include "../Engine.h"
#include "../GameplayConcepts/SceneManager.h"
#include "../GameplayConcepts/Scene.h"


Engine* GlobalFunctionLibrary::GetEngine()
{

	if (!Engine::IsInstanced())
	{
		Err() << "Engine is not instanced!" << std::endl;
		return nullptr;
	}
	
	return Engine::GetInstance();
}

Config* GlobalFunctionLibrary::GetConfig()
{
	Engine* engine = GetEngine();
	Config* config = engine->GetConfig();
	if (config == nullptr)
	{
		Err() << "Config is not initialized!" << std::endl;
		return nullptr;
	}
	return config;
}

WindowPlatform* GlobalFunctionLibrary::GetWindowPlatform()
{
	Engine* engine = GetEngine();
	WindowPlatform* windowPlatform = engine->GetWindowPlatform();

	if (windowPlatform == nullptr)
	{
		Err() << "Window Platform is not initialized!" << std::endl;
		return nullptr;
	}

	return windowPlatform;
}

GLFWwindow* GlobalFunctionLibrary::GetWindow()
{
	WindowPlatform* windowPlatform = GetWindowPlatform();

	if (windowPlatform == nullptr)
	{
		Err() << "Window Platform is not initialized!" << std::endl;
		return nullptr;
	}

	return windowPlatform->GetWindow();
}

VulkanDevice* GlobalFunctionLibrary::GetVulkanDevice()
{
	VulkanPlatform* vulkanPlatform = GetVulkanPlatform();

	return vulkanPlatform->GetDevice();
}

VulkanPlatform* GlobalFunctionLibrary::GetVulkanPlatform()
{
	Engine* engine = GetEngine();
	VulkanPlatform* vulkanPlatform = engine->GetVulkanPlatform();

	if (vulkanPlatform == nullptr)
	{
		Err() << "VulkanPlatform is not initialized!" << std::endl;
		return nullptr;
	}

	return vulkanPlatform;
}

VulkanData* GlobalFunctionLibrary::GetVulkanData()
{
	VulkanPlatform* vulkanPlatform = GetVulkanPlatform();

	if (vulkanPlatform == nullptr)
	{
		Err() << "VulkanPlatform is not initialized!" << std::endl;
		return nullptr;
	}

	return &vulkanPlatform->GetVulkanData();
}


Scene* GlobalFunctionLibrary::GetCurrentScene()
{
	Engine* engine = GetEngine();

	SceneManager* sceneManager = engine->GetSceneManager();

	if(sceneManager == nullptr)
	{
		Err() << "SceneManager is not initialized!" << std::endl;
		return nullptr;
	}

	return sceneManager->GetCurrentScene();
}

AssetDataManager* GlobalFunctionLibrary::GetAssetDataManager()
{
	Engine* engine = GetEngine();
	return engine->GetAssetDataManager();
}
