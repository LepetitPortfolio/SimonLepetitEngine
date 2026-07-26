#include "VulkanSimpleRenderSystem.h"

#include "../Core/GlobalFunctionLibrary.h"
#include "../Graphics/Shader.h"

template<typename T>
void VulkanSimpleRenderSystem::Init(ShaderSettings<T> _ShaderSettings, VkDescriptorSetLayout _DescriptorSetLayout)
{
	m_ShaderStageFlags = _ShaderSettings.ShaderStageFlags;
	CreatePipelineLayout(_DescriptorSetLayout);

	switch (m_ShaderStageFlags)
	{
	case VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT:
	{
		CreateVertFragPipeline<T>(_ShaderSettings.RenderPass, _ShaderSettings.ShaderCode[VK_SHADER_STAGE_VERTEX_BIT], _ShaderSettings.ShaderCode[VK_SHADER_STAGE_FRAGMENT_BIT]);
		break;
	}
	default:
		break;
	}

}

template<typename T>
void VulkanSimpleRenderSystem::CreateVertFragPipeline(VkRenderPass _RenderPass, const std::vector<char>* _VertCode, const std::vector<char>* _FragCode)
{
	assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo pipelineConfig{};
	VulkanPipeline::DefaultPipelineConfigInfo<T>(pipelineConfig);
	pipelineConfig.RenderPass = _RenderPass;
	pipelineConfig.PipelineLayout = m_PipelineLayout;
	m_Pipeline = std::make_unique<VulkanPipeline>(_VertCode, _FragCode, pipelineConfig);
}