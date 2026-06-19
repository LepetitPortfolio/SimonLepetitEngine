#pragma once
#include "System/WindowPlatform.h"
#include "System/VulkanPlatform.h"
#include "System/Renderer.h"
#include "GameplayConcepts/SceneManager.h"
#include "System/AssetDataManager.h"

#include <memory>

class Engine
{
public:

	static constexpr int WIDTH = 1600;
	static constexpr int HEIGHT = 900;

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

private:

	static std::unique_ptr<Engine> m_Engine;

	std::unique_ptr <WindowPlatform> m_WindowPlatform;
	std::unique_ptr <VulkanPlatform> m_VulkanPlatform;
	std::unique_ptr <Renderer> m_Renderer;

	std::unique_ptr <SceneManager> m_SceneManager;
	std::unique_ptr <AssetDataManager> m_AssetDataManager;

	class CameraBase* m_MainCamera = nullptr;
	bool m_IsInitialized = false;

};