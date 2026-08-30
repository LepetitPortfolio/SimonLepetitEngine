#include "Engine.h"
#include "Common/Error.h"
#include "Common/Time.h"
#include "GameplayConcepts/CameraBase.h"
#include "Graphics/Shader.h"

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

/*/VkDescriptorBufferInfo Engine::GetBufferInfo(int _BufferIndex)
{
	if (_BufferIndex >= m_UBOBuffers.size())
	{
		return VkDescriptorBufferInfo{};
	}

	return m_UBOBuffers[_BufferIndex]->DescriptorInfo();
}*/


bool Engine::IsInstanced()
{
	return m_Engine != nullptr;
}

void Engine::Init()
{
	m_AssetDataManager = std::make_unique<AssetDataManager>();

	m_WindowPlatform = std::make_unique<WindowPlatform>();
	m_WindowPlatform->InitWindow(WIDTH, HEIGHT, "SLE");

	m_VulkanPlatform = std::make_unique<VulkanPlatform>();
	m_VulkanPlatform->InitVulkan(m_WindowPlatform.get());

	m_DefaultCamera = std::make_unique<CameraBase>();
	m_DefaultCamera->SetViewTarget(glm::vec3{ 0.f, 0.f, -10.f }, glm::vec3{ 0.f, 0.f, 0.f });
	ChangeMainCamera(m_DefaultCamera.get());


	m_InputManager = std::make_unique<InputManager>();
	m_InputManager->Init();

	m_SceneManager = std::make_unique<SceneManager>();
	m_SceneManager->Init();

	m_IsInitialized = true;

}

void  Engine::ChangeMainCamera(CameraBase* _Camera)
{
	if (_Camera)
	{
		if (m_MainCamera)
		{
			m_MainCamera->SetUsed(false);
		}

		m_MainCamera = _Camera;

		float aspect = m_VulkanPlatform->GetVulkanRenderer()->GetAspectRatio();
		m_MainCamera->SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

		m_MainCamera->SetUsed(true);

	}
	else
	{
		assert(_Camera && "Engine::ChangeMainCamera: Invalid camera pointer.");
	}
}

void Engine::RemoveMainCamera()
{
	if (m_MainCamera)
	{
		ChangeMainCamera(m_DefaultCamera.get());
	}
	else
	{
		assert(m_MainCamera && "Engine::RemoveMainCamera: No main camera to remove.");
	}
}


void Engine::MainLoop()
{
	VulkanDevice* vulkanDevice = m_VulkanPlatform->GetDevice();
	VulkanRenderer* vulkanRenderer = m_VulkanPlatform->GetVulkanRenderer();

	Time timer;

	while (!m_WindowPlatform->ShouldClose())
	{
		glfwPollEvents();

		m_InputManager->Update();
		m_SceneManager->UpdateCurrentScene(timer.GetTimeInSeconds());

		m_VulkanPlatform->DrawFrame(m_MainCamera, timer.GetTimeInSeconds());
		
	}

	m_VulkanPlatform->WaitIdle();
}

void Engine::Cleanup()
{
	if (!m_IsInitialized)
	{
		return;
	}	
	
	m_InputManager.reset();

	m_SceneManager.reset();

	m_AssetDataManager.reset();

	m_VulkanPlatform.reset();
	m_WindowPlatform.reset();
	m_IsInitialized = false;
}

