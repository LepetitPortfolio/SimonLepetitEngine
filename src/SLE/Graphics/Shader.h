#pragma once
#include <vulkan/vulkan.h>

#include "../System/VulkanPipeline.h"
#include "../System/VulkanStructs.h"

#include "../Core/AssetData.h"

#include <map>
#include <memory>
#include <string>
#include <vector>


class Shader : public AssetData
{
public:
	
	Shader(ShaderSettings _ShaderSettings);
	
	virtual ~Shader() override;

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	std::string GetShaderName() const;
	VkPipeline GetPipeline() { return m_VulkanPipeline->GetPipeline(); }
	VkPipelineLayout GetPipelineLayout() { return m_VulkanPipeline->GetPipelineLayout(); }
	VkDescriptorSetLayout GetDescriptorSetLayout() { return m_VulkanPipeline->GetDescriptorSetLayout(); }
	std::vector<VkDescriptorSetLayoutBinding>& GetLayoutBinding() { return m_VulkanPipeline->GetLayoutBinding(); }

	void Cleanup();

	void RecreatePipeline();

	void GenertateUniformBufferDescriptorSetLayout(const VkDescriptorSetLayoutBinding _LayoutBinding, VkDescriptorSet _DescriptorSet, VkBuffer _UniformBuffers, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets);
	void GenertateCombinedImageSamplerDescriptorSetLayout(const VkDescriptorSetLayoutBinding _LayoutBinding, VkDescriptorSet _DescriptorSet, class Texture* _Texture, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets);
	void GenertateStorageBufferDescriptorSetLayout(const VkDescriptorSetLayoutBinding _LayoutBinding, VkDescriptorSet _DescriptorSet, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets);


private :

	std::string m_ShaderName;
	std::unique_ptr<VulkanPipeline> m_VulkanPipeline;

};

