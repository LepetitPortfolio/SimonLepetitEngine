#pragma once
#include "VulkanPlatform.h"
#include "VulkanStructs.h"

#include <string>
#include <map>
#include <vector>

typedef enum ShaderType
{
	VERTEX_SHADER = 0x00000001, // = VK_SHADER_STAGE_VERTEX_BIT
	TESSELLATION_CONTROL_SHADER = 0x00000002, // = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
	TESSELLATION_EVALUATION_SHADER = 0x00000004, // = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
	GEOMETRY_SHADER = 0x00000008, // = VK_SHADER_STAGE_GEOMETRY_BIT
	FRAGMENT_SHADER = 0x00000010, // = VK_SHADER_STAGE_FRAGMENT_BIT
	COMPUTE_SHADER = 0x00000020, // = VK_SHADER_STAGE_COMPUTE_BIT
	TASK_NV_SHADER = 0x00000040, // = VK_SHADER_STAGE_TASK_BIT_NV
	TASK_EXT_SHADER = TASK_NV_SHADER, // = VK_SHADER_STAGE_CALLABLE_BIT_EXT
	MESH_NV_SHADER = 0x00000080, // = VK_SHADER_STAGE_MESH_BIT_NV
	MESH_EXT_SHADER = MESH_NV_SHADER, // = VK_SHADER_STAGE_CALLABLE_BIT_EXT
	RAYGEN_KHR_SHADER = 0x00000100, // = VK_SHADER_STAGE_RAYGEN_BIT_KHR
	ANY_HIT_KHR_SHADER = 0x00000200, // = VK_SHADER_STAGE_ANY_HIT_BIT_KHR
	CLOSEST_HIT_KHR_SHADER = 0x00000400, // = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR
	MISS_KHR_SHADER = 0x00000800, // = VK_SHADER_STAGE_MISS_BIT_KHR
	INTERSECTION_KHR_SHADER = 0x00001000, // = VK_SHADER_STAGE_INTERSECTION_BIT_KHR
	CALLABLE_KHR_SHADER = 0x00002000, // = VK_SHADER_STAGE_CALLABLE_BIT_KHR
};

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

class VulkanPipeline
{
public:
	VulkanPipeline();

	~VulkanPipeline();

	VulkanPipeline(const VulkanPipeline&) = delete;
	VulkanPipeline& operator=(const VulkanPipeline&) = delete;

	VkPipeline GetPipeline() const { return m_Pipeline; }
	VkPipelineLayout GetPipelineLayout() { return m_PipelineLayout; }
	std::vector<VkDescriptorSetLayoutBinding>& GetLayoutBinding() { return m_LayoutBinding; }
	VkDescriptorSetLayout GetDescriptorSetLayout() { return m_DescriptorSetLayout; }

	void Initialize(ShaderSettings _ShaderSettings);
	void Cleanup();
	void Recreate();

	void Bind(VkCommandBuffer _CommandBuffer);

	static void EnableAlphaBlending(PipelineConfigInfo& _ConfigInfo);

private:

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

	std::vector<VkPipelineShaderStageCreateInfo> m_PipelineShaderInfo;
	std::vector<VkVertexInputBindingDescription> m_VertexInputBindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> m_VertexInputAttributeDescriptions;

	std::vector<VkDescriptorSetLayoutBinding> m_LayoutBinding;

	void CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& _Bindings);
	void CreatePipeline(const std::vector<VkPipelineShaderStageCreateInfo>& _ShaderStagesCreateInfo, const PipelineConfigInfo& _ConfigInfo);
	PipelineConfigInfo CreatePipelineConfigInfo();

};

