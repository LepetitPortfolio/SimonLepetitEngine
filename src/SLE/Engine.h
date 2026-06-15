#pragma once
#include "System/WindowPlatform.h"
#include "System/VulkanPlatform.h"
#include "GameplayConcepts/SceneManager.h"
#include "System/AssetDataManager.h"

#include <memory>

class Engine
{
public:
	Engine();
	~Engine();

	static Engine* GetInstance();

	WindowPlatform* GetWindowPlatform() { return m_WindowPlatform.get(); }

	VulkanPlatform* GetVulkanPlatform() { return m_VulkanPlatform.get(); }

	SceneManager* GetSceneManager() { return m_SceneManager.get(); }

	AssetDataManager* GetAssetDataManager() { return m_AssetDataManager.get(); }

	static bool IsInstanced();

	void Init();

	void MainLoop();

	void Cleanup();

	void ClearEngine();

private:


	static std::unique_ptr<Engine> m_Engine;

	std::unique_ptr <WindowPlatform> m_WindowPlatform;
	std::unique_ptr <VulkanPlatform> m_VulkanPlatform;

	std::unique_ptr <SceneManager> m_SceneManager;
	std::unique_ptr <AssetDataManager> m_AssetDataManager;

};