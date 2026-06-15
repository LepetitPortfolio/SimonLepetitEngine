#include "Engine.h"
#include "Common/Error.h"


std::unique_ptr<Engine> Engine::m_Engine;

Engine::Engine()
{
}

Engine::~Engine()
{
	Cleanup();
}

Engine* Engine::GetInstance()
{
	if (m_Engine == nullptr)
	{
		m_Engine = std::make_unique<Engine>();
		m_Engine->Init();
	}
	return m_Engine.get();
}

bool Engine::IsInstanced()
{
	return m_Engine != nullptr;
}

void Engine::Init()
{
	m_WindowPlatform = std::make_unique<WindowPlatform>();
	m_VulkanPlatform = std::make_unique<VulkanPlatform>();
	m_WindowPlatform->InitWindow();
	m_VulkanPlatform->InitVulkan(m_WindowPlatform.get());
	m_SceneManager = std::make_unique<SceneManager>();
	m_SceneManager->Init();
	m_AssetDataManager = std::make_unique<AssetDataManager>();

}

void Engine::MainLoop()
{
	while (!glfwWindowShouldClose(m_WindowPlatform->GetWindow()))
	{
		glfwPollEvents();
		m_SceneManager->UpdateCurrentScene();
		m_VulkanPlatform->DrawFrame();
	}

	m_VulkanPlatform->WaitIdle();
}

void Engine::Cleanup()
{
	m_VulkanPlatform->Cleanup();
	m_WindowPlatform->Cleanup();
}

void Engine::ClearEngine()
{
	Cleanup();
	m_SceneManager.reset();
	m_AssetDataManager.reset();
}
