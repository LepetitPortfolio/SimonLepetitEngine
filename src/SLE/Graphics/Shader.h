#pragma once
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>

#include <vector>
#include <string>

struct PipelineConfigInfo 
{
	PipelineConfigInfo() = default;
	PipelineConfigInfo(const PipelineConfigInfo&) = delete;
	PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

	std::vector<VkVertexInputBindingDescription> BindingDescriptions{};
	std::vector<VkVertexInputAttributeDescription> AttributeDescriptions{};
	VkPipelineViewportStateCreateInfo ViewportInfo;
	VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo RasterizationInfo;
	VkPipelineMultisampleStateCreateInfo MultisampleInfo;
	VkPipelineColorBlendAttachmentState ColorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
	std::vector<VkDynamicState> DynamicStateEnables;
	VkPipelineDynamicStateCreateInfo DynamicStateInfo;
	VkPipelineLayout PipelineLayout = nullptr;
	VkRenderPass RenderPass = nullptr;
	uint32_t Subpass = 0;
};


class Shader
{
public:
	Shader();
	Shader(std::string _ShaderName);
	~Shader();

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;


	std::string GetShaderName() const;
	VkDescriptorSetLayout& GetDescriptorSetLayout();
	VkPipelineLayout& GetPipelineLayout();
	VkPipeline& GetPipeline();

	std::vector<VkDescriptorSetLayoutBinding>* GetLayoutBinding();
	void SetLayoutBinding(std::vector<VkDescriptorSetLayoutBinding> _LayoutBinding);

	template<typename T>
	static void DefaultPipelineConfigInfo(PipelineConfigInfo& _ConfigInfo);
	static void EnableAlphaBlending(PipelineConfigInfo& _ConfigInfo);

	bool FindLayoutBinding(VkDescriptorType _DescriptorType);

	void GenertateUniformBufferDescriptorSetLayout(const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, uint32_t _FrameIndex, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets);
	void GenertateCombinedImageSamplerDescriptorSetLayout(const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, class Texture* _Texture, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets);
	void GenertateStorageBufferDescriptorSetLayout(const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets);

	void Cleanup();

private :

	std::string m_ShaderName;

	// --------------------------------------------------------------------
	// Layout des sets de descripteurs : définit la structure des ressources accessibles dans les shaders.
	// Spécifie les bindings (ex: buffer uniforme en binding 0, texture en binding 1) et leurs types.
	VkDescriptorSetLayout m_DescriptorSetLayout;

	// --------------------------------------------------------------------
	// Layout du pipeline graphique : définit les ressources (descripteurs, push constants) utilisées par le pipeline.
	// Contient les layouts des sets de descripteurs et les plages de push constants.
	VkPipelineLayout m_PipelineLayout;

	// --------------------------------------------------------------------
	// Pipeline graphique : définit toutes les étapes fixes du rendu (shaders, assemblage des primitives, rasterization, etc.).
	// Représente le "chemin" que suivent les données pour être transformées en pixels à l'écran.
	VkPipeline m_Pipeline;

	std::vector<VkDescriptorSetLayoutBinding> m_LayoutBinding;

};

#include "Shader.inl"
