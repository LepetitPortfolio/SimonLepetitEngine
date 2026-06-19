#include "Shader.h"
#include "../Core/GlobalFunctionLibrary.h"

#include "Texture.h"

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

std::vector<VkDescriptorSetLayoutBinding>* Shader::GetLayoutBinding()
{
	return &m_LayoutBinding;
}

void Shader::SetLayoutBinding(std::vector<VkDescriptorSetLayoutBinding> _LayoutBinding)
{
	m_LayoutBinding = _LayoutBinding;
}

void Shader::EnableAlphaBlending(PipelineConfigInfo& _ConfigInfo)
{
	_ConfigInfo.ColorBlendAttachment.blendEnable = VK_TRUE;
	_ConfigInfo.ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |	VK_COLOR_COMPONENT_A_BIT;
	_ConfigInfo.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	_ConfigInfo.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	_ConfigInfo.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	_ConfigInfo.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	_ConfigInfo.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	_ConfigInfo.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

bool Shader::FindLayoutBinding(VkDescriptorType _DescriptorType)
{
	int index = 0;
	bool found = false;
	while ((!found) && (index < m_LayoutBinding.size()))
	{
		if (m_LayoutBinding[index].descriptorType == _DescriptorType)
		{
			found = true;
		}
		index++;
	}
	return found;
}

void Shader::GenertateUniformBufferDescriptorSetLayout(const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, uint32_t _FrameIndex, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets)
{
	VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo{};
	bufferInfo->buffer = GlobalFunctionLibrary::GetVulkanData()->UniformBuffers[_FrameIndex];
	bufferInfo->offset = 0;
	bufferInfo->range = sizeof(UniformBufferObject);

	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = _DescriptorSet;
	writeDescriptorSet.dstBinding = _LayoutBinding.binding;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorType = _LayoutBinding.descriptorType;
	writeDescriptorSet.descriptorCount = _LayoutBinding.descriptorCount;
	writeDescriptorSet.pBufferInfo = bufferInfo;

	_WriteDescriptorSets.push_back(writeDescriptorSet);
}

void Shader::GenertateCombinedImageSamplerDescriptorSetLayout(const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, Texture* _Texture, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets)
{
	if (_Texture)
	{
		VkDescriptorImageInfo* imageInfo = new VkDescriptorImageInfo{};
		imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo->imageView = _Texture->GetTextureImageView();
		imageInfo->sampler = _Texture->GetTextureSampler();

		VkWriteDescriptorSet writeDescriptorSet{};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = _DescriptorSet;
		writeDescriptorSet.dstBinding = _LayoutBinding.binding;
		writeDescriptorSet.dstArrayElement = 0;
		writeDescriptorSet.descriptorType = _LayoutBinding.descriptorType;
		writeDescriptorSet.descriptorCount = _LayoutBinding.descriptorCount;
		writeDescriptorSet.pImageInfo = imageInfo;

		_WriteDescriptorSets.push_back(writeDescriptorSet);
	}
}

void Shader::GenertateStorageBufferDescriptorSetLayout(const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets)
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