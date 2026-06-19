#include "Engine.h"
#include "Common/Error.h"
#include "Common/Time.h"
#include "GameplayConcepts/CameraBase.h"


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
	m_WindowPlatform->InitWindow(WIDTH, HEIGHT, "SLE");

	m_VulkanPlatform = std::make_unique<VulkanPlatform>(m_WindowPlatform.get());
	
	m_Renderer = std::make_unique<Renderer>(m_WindowPlatform.get(), m_VulkanPlatform.get());

	m_SceneManager = std::make_unique<SceneManager>();
	m_SceneManager->Init();
	m_AssetDataManager = std::make_unique<AssetDataManager>();
	m_IsInitialized = true;

}

void Engine::MainLoop()
{
	auto currentTime = std::chrono::high_resolution_clock::now();

	while (!m_WindowPlatform->ShouldClose())
	{
		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;

		cameraController.moveInPlaneXZ(m_WindowPlatform->GetWindow(), frameTime, viewerObject);
		m_MainCamera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

		float aspect = m_Renderer->GetAspectRatio();
		m_MainCamera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

		if (auto commandBuffer = m_Renderer->BeginFrame())
		{
			int frameIndex = m_Renderer->GetFrameIndex();
			FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, m_MainCamera, globalDescriptorSets[frameIndex], gameObjects };

			// update
			GlobalUbo ubo{};
			ubo.projection = m_MainCamera.GetProjection();
			ubo.view = m_MainCamera.GetView();
			ubo.inverseView = m_MainCamera.GetInverseView();
			pointLightSystem.update(frameInfo, ubo);
			uboBuffers[frameIndex]->writeToBuffer(&ubo);
			uboBuffers[frameIndex]->flush();

			// render
			m_Renderer->BeginSwapChainRenderPass(commandBuffer);

			// order here matters
			simpleRenderSystem.renderGameObjects(frameInfo);
			pointLightSystem.render(frameInfo);

			m_Renderer->EndSwapChainRenderPass(commandBuffer);
			m_Renderer->EndFrame();
		}
	}
}

void Engine::Cleanup()
{
	if (!m_IsInitialized)
	{
		return;
	}

	m_SceneManager->Cleanup();
	m_SceneManager.reset();
	m_AssetDataManager->ClearAllData();
	m_AssetDataManager.reset();
	m_VulkanPlatform.reset();
	m_WindowPlatform->Cleanup();
	m_WindowPlatform.reset();
	m_IsInitialized = false;
}
