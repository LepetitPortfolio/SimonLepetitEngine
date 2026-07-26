#pragma once

#include "VulkanPlatform.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanStructs.h"


#include <memory>
#include <vector>

template<typename T>
struct ShaderSettings;

class VulkanSimpleRenderSystem
{
public:

	VulkanSimpleRenderSystem();

	~VulkanSimpleRenderSystem();

	VulkanSimpleRenderSystem(const VulkanSimpleRenderSystem&) = delete;
	VulkanSimpleRenderSystem& operator=(const VulkanSimpleRenderSystem&) = delete;

	//void RenderGameObjects(VulkanFrameInfo& _FrameInfo);
	void RenderGameObjects(VkCommandBuffer _CommandBuffer, VkDescriptorSet _DescriptorSet, class Transform* _Transform, class Model* _Model);

	template<typename T>
	void Init(ShaderSettings<T> _ShaderSettings, VkDescriptorSetLayout _DescriptorSetLayout);

private:

	std::unique_ptr<VulkanPipeline> m_Pipeline;
	VkPipelineLayout m_PipelineLayout;

	VkShaderStageFlags m_ShaderStageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

	void CreatePipelineLayout(VkDescriptorSetLayout _DescriptorSetLayout);

	template<typename T>
	void CreateVertFragPipeline(VkRenderPass _RenderPass, const std::vector<char>* _VertCode, const std::vector<char>* _FragCode);

};

#include "VulkanSimpleRenderSystem.inl"