#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Engine.h"
#include "../Core/Transform.h"
#include "../Core/GlobalFunctionLibrary.h"
#include "../System/VulkanSwapchain.h"
#include "../Common/Error.h"

#include <cassert>

Mesh::Mesh(Model* _Model, Shader* _Shader, Texture* _Texture)
{
	m_Model = _Model;
	m_Shader = _Shader;
	m_Texture = _Texture;

	CreateUniformBuffers();

	CreateDescriptorPool();

	UpdateDescriptorSets();
}

Mesh::~Mesh()
{
}

void Mesh::Draw(Transform* _Transform, VulkanFrameInfo& _FrameInfo)
{
	UpdateUniforms(_Transform, _FrameInfo.Camera, _FrameInfo.CurrentFrameIndexInFlight);

	VulkanCommandManager* commandManager = GlobalFunctionLibrary::GetVulkanPlatform()->GetCommandManager();
	VulkanRenderer* vulkanRenderer = GlobalFunctionLibrary::GetVulkanPlatform()->GetVulkanRenderer();
	VulkanSwapchain* vulkanSwapchain = GlobalFunctionLibrary::GetVulkanPlatform()->GetVulkanSwapchain();
	VulkanData& vulkanData = GlobalFunctionLibrary::GetVulkanPlatform()->GetVulkanData();
	VkFramebuffer framebuffer = GlobalFunctionLibrary::GetVulkanPlatform()->GetFrameBuffer(_FrameInfo.FrameIndex);

	commandManager->RecordCommandBuffer(_FrameInfo.CommandBuffer, _FrameInfo.FrameIndex, vulkanRenderer->GetRenderPass(),
										framebuffer, vulkanSwapchain->GetExtent(), m_Shader->GetPipeline(), m_Shader->GetPipelineLayout(),
										m_Model->GetVertexBuffer(), m_Model->GetIndexBuffer(), m_DescriptorSets, vulkanData.CurrentFrameIndexInFlight, 
										static_cast<uint32_t>(m_Model->GetIndices().size()));
}

void Mesh::Cleanup()
{
	VulkanBufferManager* bufferManager = GlobalFunctionLibrary::GetVulkanPlatform()->GetBufferManager();
	VkDevice device = GlobalFunctionLibrary::GetVulkanPlatform()->GetDevice()->GetLogicalDevice();

	for (size_t i = 0; i < m_UniformBuffers.size(); i++)
	{
		if (m_UniformBuffersMemory[i] != VK_NULL_HANDLE)
		{
			vkUnmapMemory(device, m_UniformBuffersMemory[i]);
		}
		bufferManager->DestroyBuffer(m_UniformBuffers[i], m_UniformBuffersMemory[i]);
	}
}

void Mesh::UpdateDescriptorSets()
{
	Engine* engine = GlobalFunctionLibrary::GetEngine();

	if ((engine == nullptr) || (m_Shader == nullptr))
	{
		return;
	}
	
	VulkanDevice* vulkanDevice = GlobalFunctionLibrary::GetVulkanPlatform()->GetDevice();

	int maxFramesInFlight = GlobalFunctionLibrary::GetConfig()->MaxFramesInFlight;
	std::vector<VkDescriptorSetLayout> setLayouts(maxFramesInFlight, m_Shader->GetDescriptorSetLayout());

	m_DescriptorSets = std::vector<VkDescriptorSet>(maxFramesInFlight);

	VkDescriptorSetAllocateInfo setAllocateInfo{};
	setAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	setAllocateInfo.descriptorPool = m_DescriptorPool;
	setAllocateInfo.descriptorSetCount = static_cast<uint32_t>(maxFramesInFlight);
	setAllocateInfo.pSetLayouts = setLayouts.data();

	if (vkAllocateDescriptorSets(vulkanDevice->GetLogicalDevice(), &setAllocateInfo, m_DescriptorSets.data()) != VK_SUCCESS)
	{
		Err() << "failed to allocate descriptor sets!" << std::endl;
	}

	for (size_t frameIndex = 0; frameIndex < maxFramesInFlight; frameIndex++)
	{
		auto layoutBindings = m_Shader->GetLayoutBinding();

		std::vector<VkWriteDescriptorSet> writeDescriptorSets{};

		for (auto& layoutBinding  : layoutBindings)
		{
			
			switch (layoutBinding.descriptorType)
			{
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:

				m_Shader->GenertateUniformBufferDescriptorSetLayout(layoutBinding, m_DescriptorSets[frameIndex], GetUniformBuffer(frameIndex), writeDescriptorSets);
				break;

			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:

				m_Shader->GenertateCombinedImageSamplerDescriptorSetLayout(layoutBinding, m_DescriptorSets[frameIndex], m_Texture, writeDescriptorSets);
				break;

			case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:

				//m_Shader->GenertateStorageBufferDescriptorSetLayout(lBinding, m_DescriptorSets[frameIndex], writeDescriptorSets);

				break;

			};
		}

		vkUpdateDescriptorSets(vulkanDevice->GetLogicalDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}
	
}

void Mesh::CreateUniformBuffers()
{
	int maxFramesInFlight = GlobalFunctionLibrary::GetConfig()->MaxFramesInFlight;
	VulkanBufferManager* bufferManager = GlobalFunctionLibrary::GetVulkanPlatform()->GetBufferManager();

	bufferManager->CreateUniformBuffer(maxFramesInFlight, m_UniformBuffers, m_UniformBuffersMemory, m_UniformBuffersMapped);

}

void Mesh::CreateDescriptorPool()
{
	int maxFramesInFlight = GlobalFunctionLibrary::GetConfig()->MaxFramesInFlight;
	VulkanDevice* vulkanDevice = GlobalFunctionLibrary::GetVulkanPlatform()->GetDevice();

	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(maxFramesInFlight);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(maxFramesInFlight);

	VkDescriptorPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolCreateInfo.pPoolSizes = poolSizes.data();
	poolCreateInfo.maxSets = static_cast<uint32_t>(maxFramesInFlight);

	if (vkCreateDescriptorPool(vulkanDevice->GetLogicalDevice(), &poolCreateInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
	{
		Err() << "failed to create descriptor pool!" << std::endl;
	}
}

void Mesh::UpdateUniforms(Transform* _Transform, CameraBase* _Camera, uint32_t _ImageIndex)
{

	UniformBufferObject ubo{};

	glm::mat4 translation = glm::translate(glm::mat4(1.0f), _Transform->GetPosition());
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(_Transform->GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(_Transform->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(_Transform->GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 scaling = glm::scale(glm::mat4(1.0f), _Transform->GetScale());
	ubo.Model = translation * rotation * scaling;

	//ubo.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//ubo.Proj = glm::perspective(glm::radians(45.0f), vulkanSwapchain_->getExtent().width / (float)vulkanSwapchain_->getExtent().height, 0.1f, 10.0f);
	ubo.View = _Camera->GetView();
	ubo.InverseView = _Camera->GetInverseView();
	ubo.Projection = _Camera->GetProjection();
	ubo.Projection[1][1] *= -1;

	memcpy(m_UniformBuffersMapped[_ImageIndex], &ubo, sizeof(ubo));
}
