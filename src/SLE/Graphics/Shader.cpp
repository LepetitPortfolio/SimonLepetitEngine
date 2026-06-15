#include "Shader.h"
#include "../Core/GlobalFunctionLibrary.h"

Shader::Shader()
{
}

Shader::Shader(std::string _ShaderName)
{
	m_ShaderName = _ShaderName;
}

Shader::~Shader()
{
	Cleanup();
}


std::string Shader::GetShaderName() const
{
	return m_ShaderName;
}

VkDescriptorSetLayout& Shader::GetDescriptorSetLayout()
{
	return m_DescriptorSetLayout;
}

VkPipeline& Shader::GetPipeline()
{
	return m_Pipeline;
}

VkPipelineLayout& Shader::GetPipelineLayout()
{
	return m_PipelineLayout;
}

void Shader::Cleanup()
{
	vkDestroyPipeline(GlobalFunctionLibrary::GetVulkanDevice(), m_Pipeline, nullptr);
	vkDestroyPipelineLayout(GlobalFunctionLibrary::GetVulkanDevice(), m_PipelineLayout, nullptr);

	vkDestroyDescriptorSetLayout(GlobalFunctionLibrary::GetVulkanDevice(), m_DescriptorSetLayout, nullptr);
}