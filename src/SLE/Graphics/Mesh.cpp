#include "Mesh.h"
#include "../Engine.h"
#include "../Core/GlobalFunctionLibrary.h"
#include "../System/VulkanSwapChain.h"

#include <cassert>

Mesh::Mesh(Model* _Model, Shader* _Shader, Texture* _Texture)
{
	m_Model = _Model;
	m_Shader = _Shader;
	m_Texture = _Texture;

	UpdateDescriptorSets();
}

void Mesh::Draw(Transform* _Transform, VulkanFrameInfo _FrameInfo)
{
	if (m_Shader)
	{
		m_Shader->RenderGameObjects(_FrameInfo.CommandBuffer, m_DescriptorSets[_FrameInfo.FrameIndex], _Transform, m_Model);
	}
}

void Mesh::UpdateDescriptorSets()
{
	Engine* engine = GlobalFunctionLibrary::GetEngine();

	if ((engine == nullptr) || (m_Shader == nullptr))
	{
		return;
	}

	m_DescriptorSets = std::vector<VkDescriptorSet>(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < m_DescriptorSets.size(); i++)
	{
		auto bufferInfo = engine->GetBufferInfo(i);
		VulkanDescriptorWriter(m_Shader->GetDescriptorSetLayout(), m_Shader->GetDescriptorPool()).WriteBuffer(0, &bufferInfo).Build(m_DescriptorSets[i]);
	}


	for (size_t frameIndex = 0; frameIndex < MAX_FRAMES_IN_FLIGHT; frameIndex++)
	{
		auto layoutBindings = m_Shader->GetDescriptorSetLayout()->GetBindings();

		std::vector<VkWriteDescriptorSet> writeDescriptorSets{};

		for (auto& layoutBinding  : layoutBindings)
		{
			
			switch (layoutBinding.second.descriptorType)
			{
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:

				m_Shader->GenertateDescriptorSetLayout(writeDescriptorSets, layoutBinding.second, m_DescriptorSets[frameIndex], frameIndex);

				break;

			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:

				m_Shader->GenertateDescriptorSetLayout(writeDescriptorSets, layoutBinding.second, m_DescriptorSets[frameIndex], m_Texture);
				break;

			case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:

				//m_Shader->GenertateStorageBufferDescriptorSetLayout(lBinding, m_DescriptorSets[frameIndex], writeDescriptorSets);

				break;

			};
		}

		vkUpdateDescriptorSets(GlobalFunctionLibrary::GetVulkanDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);		
	}
	
}
