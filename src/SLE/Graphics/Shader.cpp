#include "Shader.h"
#include "../Core/GlobalFunctionLibrary.h"
#include "../Engine.h"

#include "Texture.h"


Shader::~Shader()
{
	Cleanup();
}


std::string Shader::GetShaderName() const
{
	return m_ShaderName;
}


void Shader::RenderGameObjects(VkCommandBuffer _CommandBuffer, VkDescriptorSet _DescriptorSet, Transform* _Transform, Model* _Model)
{
	m_SimpleRenderSystem->RenderGameObjects(_CommandBuffer, _DescriptorSet, _Transform, _Model);
}

void Shader::GenertateDescriptorSetLayout(std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets, const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, uint32_t _FrameIndex)
{
	Engine* engine = GlobalFunctionLibrary::GetEngine();

	if (engine)
	{
		VulkanBuffer* buffer = engine->GetVulkanBuffer(_FrameIndex);

		VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo{};
		bufferInfo->buffer = buffer->GetBuffer();
		bufferInfo->offset = 0;
		bufferInfo->range = buffer->GetBufferSize();

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
}

void Shader::GenertateDescriptorSetLayout(std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets, const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, Texture* _Texture)
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


void Shader::Cleanup()
{
	m_SimpleRenderSystem.reset();
	m_DescriptorSetLayout.reset();
	m_DescriptorPool.reset();
}