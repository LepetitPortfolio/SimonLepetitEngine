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
