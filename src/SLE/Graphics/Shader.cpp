#include "Shader.h"
#include "../Core/GlobalFunctionLibrary.h"
#include "../Engine.h"

#include "../Common/Error.h"

#include "Textures/Texture.h"

Shader::Shader(ShaderSettings _ShaderSettings)
{
	m_ShaderName = _ShaderSettings.ShaderName;

	m_VulkanPipeline = std::make_unique<VulkanPipeline>();
	m_VulkanPipeline->Initialize(_ShaderSettings);

	GlobalFunctionLibrary::GetAssetDataManager()->AddData(this);
}

Shader::~Shader()
{
	Cleanup();
}


std::string Shader::GetShaderName() const
{
	return m_ShaderName;
}


void Shader::Cleanup()
{
	m_VulkanPipeline->Cleanup();
}

void Shader::RecreatePipeline()
{
	m_VulkanPipeline->Recreate();
}

void Shader::GenertateUniformBufferDescriptorSetLayout(const VkDescriptorSetLayoutBinding _LayoutBinding, VkDescriptorSet _DescriptorSet, VkDescriptorBufferInfo& _DescriptorBufferInfo, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets)
{
	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = _DescriptorSet;
	writeDescriptorSet.dstBinding = _LayoutBinding.binding;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorType = _LayoutBinding.descriptorType;
	writeDescriptorSet.descriptorCount = _LayoutBinding.descriptorCount;
	writeDescriptorSet.pBufferInfo = &_DescriptorBufferInfo;

	_WriteDescriptorSets.push_back(writeDescriptorSet);
}

void Shader::GenertateCombinedImageSamplerDescriptorSetLayout(const VkDescriptorSetLayoutBinding _LayoutBinding, VkDescriptorSet _DescriptorSet, VkDescriptorImageInfo& _DescriptorImageInfo, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets)
{
	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = _DescriptorSet;
	writeDescriptorSet.dstBinding = _LayoutBinding.binding;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorType = _LayoutBinding.descriptorType;
	writeDescriptorSet.descriptorCount = _LayoutBinding.descriptorCount;
	writeDescriptorSet.pImageInfo = &_DescriptorImageInfo;

	_WriteDescriptorSets.push_back(writeDescriptorSet);
	
}

void Shader::GenertateStorageBufferDescriptorSetLayout(const VkDescriptorSetLayoutBinding _LayoutBinding, VkDescriptorSet _DescriptorSet, VkDescriptorBufferInfo& _DescriptorBufferInfo, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets)
{
	/*VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = ;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof();

	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = _DescriptorSet;
	writeDescriptorSet.dstBinding = _LayoutBinding.binding;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorType = _LayoutBinding.descriptorType;
	writeDescriptorSet.descriptorCount = _LayoutBinding.descriptorCount;
	writeDescriptorSet.pBufferInfo = &bufferInfo;

	_WriteDescriptorSets.push_back(writeDescriptorSet);*/
}
