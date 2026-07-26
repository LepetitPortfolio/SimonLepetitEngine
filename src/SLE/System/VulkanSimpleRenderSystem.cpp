#include "VulkanSimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "../Core/GlobalFunctionLibrary.h"
#include "../Core/Transform.h"
#include "../Graphics/Models/Model.h"

#include <array>
#include <cassert>
#include <stdexcept>

struct VulkanSimplePushConstantData 
{
	glm::mat4 ModelMatrix{ 1.f };
	glm::mat4 NormalMatrix{ 1.f };
};

VulkanSimpleRenderSystem::VulkanSimpleRenderSystem(){}

VulkanSimpleRenderSystem::~VulkanSimpleRenderSystem()
{
	vkDestroyPipelineLayout(GlobalFunctionLibrary::GetVulkanDevice(), m_PipelineLayout, nullptr);
}

void VulkanSimpleRenderSystem::RenderGameObjects(VkCommandBuffer _CommandBuffer, VkDescriptorSet _DescriptorSet, Transform* _Transform, Model* _Model)
{
	m_Pipeline->Bind(_CommandBuffer);

	vkCmdBindDescriptorSets(_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &_DescriptorSet, 0, nullptr);

	if (_Model != nullptr)
	{
		VulkanSimplePushConstantData push{};
		push.ModelMatrix = _Transform->TransformMatrix();
		push.NormalMatrix = _Transform->TransformNormalMatrix();

		vkCmdPushConstants(_CommandBuffer, m_PipelineLayout, m_ShaderStageFlags, 0, sizeof(VulkanSimplePushConstantData), &push);

		_Model->Bind(_CommandBuffer);
		_Model->Draw(_CommandBuffer);
	}
}

void VulkanSimpleRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout _DescriptorSetLayout)
{
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = m_ShaderStageFlags;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(VulkanSimplePushConstantData);

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ _DescriptorSetLayout };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	if (vkCreatePipelineLayout(GlobalFunctionLibrary::GetVulkanDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
}