#pragma once
#include <vulkan/vulkan.h>

#include "../System/VulkanStructs.h"

#include "../Core/AssetData.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

struct ShaderCodeSettings
{
	std::vector<char> ShaderCode;
	VkShaderModule ShaderModule = VK_NULL_HANDLE;
};

struct ShaderSettings
{
	VkShaderStageFlags ShaderStageFlags;
	std::string ShaderName;
	std::map<VkShaderStageFlagBits, ShaderCodeSettings> ShaderCodeInfos;
	std::vector<VkDescriptorSetLayoutBinding> Bindings;

	VkRenderPass RenderPass;

	std::vector<VkPipelineShaderStageCreateInfo> PipelineShaderInfo;
	std::vector<VkVertexInputBindingDescription> VertexInputBindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> VertexInputAttributeDescriptions;

};

class Shader : public AssetData
{
public:
	
	Shader(ShaderSettings _ShaderSettings);
	
	virtual ~Shader() override;

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	std::string GetShaderName() const;
	VkPipeline& GetPipeline() { return m_Pipeline; }
	VkPipelineLayout& GetPipelineLayout() { return m_PipelineLayout; }
	VkDescriptorSetLayout& GetDescriptorSetLayout() { return m_DescriptorSetLayout; }
	std::vector<VkDescriptorSetLayoutBinding>& GetLayoutBinding() { return m_LayoutBinding; }

	void Cleanup();

	void RecreatePipeline();

	void GenertateUniformBufferDescriptorSetLayout(const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, VkBuffer _UniformBuffers, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets);
	void GenertateCombinedImageSamplerDescriptorSetLayout(const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, class Texture* _Texture, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets);
	void GenertateStorageBufferDescriptorSetLayout(const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets);


private :

	std::string m_ShaderName;

	// --------------------------------------------------------------------
	// Layout des sets de descripteurs : définit la structure des ressources accessibles dans les shaders.
	// Spécifie les bindings (ex: buffer uniforme en binding 0, texture en binding 1) et leurs types.
	VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;

	// --------------------------------------------------------------------
	// Layout du pipeline graphique : définit les ressources (descripteurs, push constants) utilisées par le pipeline.
	// Contient les layouts des sets de descripteurs et les plages de push constants.
	VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

	// --------------------------------------------------------------------
	// Pipeline graphique : définit toutes les étapes fixes du rendu (shaders, assemblage des primitives, rasterization, etc.).
	// Représente le "chemin" que suivent les données pour être transformées en pixels à l'écran.
	VkPipeline m_Pipeline = VK_NULL_HANDLE;

	std::vector<VkDescriptorSetLayoutBinding> m_LayoutBinding;

	std::vector<VkPipelineShaderStageCreateInfo> m_PipelineShaderInfo;
	std::vector<VkVertexInputBindingDescription> m_VertexInputBindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> m_VertexInputAttributeDescriptions;


	void CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& _Bindings);
	void CreatePipeline(const std::vector<VkPipelineShaderStageCreateInfo>& _ShaderStagesCreateInfo, const PipelineConfigInfo& _ConfigInfo);


	PipelineConfigInfo CreatePipelineConfigInfo();

};

