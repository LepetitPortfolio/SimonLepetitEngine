#include "GlobalFunctionLibrary.h"

#include "../Common/Error.h"
#include "../System/VulkanPlatform.h"
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

const VkDevice GlobalFunctionLibrary::GetVulkanDevice()
{

	return GetVulkanData()->Device;
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

VulkanRenderer* GlobalFunctionLibrary::GetVulkanRenderer()
{
	Engine* engine = GetEngine();
	VulkanRenderer* vulkanRenderer = engine->GetVulkanRenderer();

	if (vulkanRenderer == nullptr)
	{
		Err() << "vulkanRenderer is not initialized!" << std::endl;
		return nullptr;
	}

	return vulkanRenderer;
}

const VulkanData* GlobalFunctionLibrary::GetVulkanData()
{	
	return GetVulkanPlatform()->GetVulkanData();
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
