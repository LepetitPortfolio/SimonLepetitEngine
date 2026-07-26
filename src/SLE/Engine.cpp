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

VkDescriptorBufferInfo Engine::GetBufferInfo(int _BufferIndex)
{
	if (_BufferIndex >= m_UBOBuffers.size())
	{
		return VkDescriptorBufferInfo{};
	}

	return m_UBOBuffers[_BufferIndex]->DescriptorInfo();
}

VulkanBuffer* Engine::GetVulkanBuffer(int _BufferIndex)
{
	if (_BufferIndex >= m_UBOBuffers.size())
	{
		return nullptr;
	}

	return m_UBOBuffers[_BufferIndex].get();
}

bool Engine::IsInstanced()
{
	return m_Engine != nullptr;
}

void Engine::Init()
{
	m_WindowPlatform = std::make_unique<WindowPlatform>();
	m_WindowPlatform->InitWindow(WIDTH, HEIGHT, "SLE");

	m_VulkanPlatform = std::make_unique<VulkanPlatform>(*m_WindowPlatform.get());
	
	m_Renderer = std::make_unique<VulkanRenderer>();


	m_DefaultCamera = std::make_unique<CameraBase>();
	ChangeMainCamera(m_DefaultCamera.get());

	m_DefaultCamera->SetViewDirection(glm::vec3{ 0.f, 0.f, -10.f }, glm::vec3{ 0.f, 0.f, 1.f });

	m_InputManager = std::make_unique<InputManager>();
	m_InputManager->Init();

	m_SceneManager = std::make_unique<SceneManager>();
	m_SceneManager->Init();
	m_AssetDataManager = std::make_unique<AssetDataManager>();

	m_UBOBuffers = std::vector<std::unique_ptr<VulkanBuffer>>(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int uboBuffersIndex = 0; uboBuffersIndex < m_UBOBuffers.size(); uboBuffersIndex++)
	{
		m_UBOBuffers[uboBuffersIndex] = std::make_unique<VulkanBuffer>(sizeof(UniformBufferObject), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		m_UBOBuffers[uboBuffersIndex]->Map();
	}


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
		m_MainCamera->SetUsed(true);

	}
	else
	{
		assert("Engine::ChangeMainCamera: Invalid camera pointer.");
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
		assert("Engine::RemoveMainCamera: No main camera to remove.");
	}
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

		m_InputManager->Update();

		float aspect = m_Renderer->GetAspectRatio();
		m_MainCamera->SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

		if (auto commandBuffer = m_Renderer->BeginFrame())
		{
			int frameIndex = m_Renderer->GetFrameIndex();
			VulkanFrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, m_MainCamera};

			// update
			UniformBufferObject ubo{};
			ubo.Projection = m_MainCamera->GetProjection();
			ubo.View = m_MainCamera->GetView();
			ubo.InverseView = m_MainCamera->GetInverseView();

			//pointLightSystem.update(frameInfo, ubo);

			m_UBOBuffers[frameIndex]->WriteToBuffer(&ubo);
			m_UBOBuffers[frameIndex]->Flush();

			// render
			m_Renderer->BeginSwapChainRenderPass(commandBuffer);

			// order here matters

			Scene* scene = GlobalFunctionLibrary::GetCurrentScene();

			if (scene)
			{
				scene->UpdateDraw(frameInfo);
			}

			/*if (m_AssetDataManager.get())
			{
				auto gameObjectManager = m_AssetDataManager->GetGameObjectManager();
				auto gameObjects = gameObjectManager->GetData();
				for (auto data : *gameObjects)
				{
					data.second->DrawGameObject(frameInfo);
				}
			}*/

			//simpleRenderSystem.RenderGameObjects(frameInfo);
			//pointLightSystem.render(frameInfo);

			m_Renderer->EndSwapChainRenderPass(commandBuffer);
			m_Renderer->EndFrame();
		}

		vkDeviceWaitIdle(m_VulkanPlatform->GetDevice());
	}
}

void Engine::Cleanup()
{
	if (!m_IsInitialized)
	{
		return;
	}

	for (int uboBuffersIndex = 0; uboBuffersIndex < m_UBOBuffers.size(); uboBuffersIndex++)
	{
		m_UBOBuffers[uboBuffersIndex].reset();
	}

	m_UBOBuffers.clear();

	m_SceneManager->Cleanup();
	m_SceneManager.reset();
	m_AssetDataManager->ClearAllData();
	m_AssetDataManager.reset();
	m_InputManager->Cleanup();
	m_InputManager.reset();
	m_Renderer.reset();
	m_VulkanPlatform.reset();
	m_WindowPlatform->Cleanup();
	m_WindowPlatform.reset();
	m_IsInitialized = false;
}

