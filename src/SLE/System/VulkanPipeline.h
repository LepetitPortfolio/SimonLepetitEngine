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

class VulkanPipeline
{
public:
	VulkanPipeline(const std::vector<char>* _VertCode, const std::vector<char>* _FragCode, const PipelineConfigInfo& _ConfigInfo);

	~VulkanPipeline();

	VulkanPipeline(const VulkanPipeline&) = delete;
	VulkanPipeline& operator=(const VulkanPipeline&) = delete;

	void Bind(VkCommandBuffer _CommandBuffer);


	template<typename T>
	static void CreatePipelineConfigInfo(PipelineConfigInfo& _ConfigInfo);

	static void EnableAlphaBlending(PipelineConfigInfo& _ConfigInfo);

private:

	VkPipeline m_GraphicsPipeline;
	std::map<ShaderType, VkShaderModule> m_ShaderModules;

	void CreateGraphicsPipeline(const std::vector<char>* _VertCode, const std::vector<char>* _FragCode, const PipelineConfigInfo& _ConfigInfo);

	void CreateShaderModule(const std::vector<char>* _Code, VkShaderModule* _ShaderModule);

	VkPipelineShaderStageCreateInfo CreateShaderStage(VkShaderStageFlagBits _ShaderStage, VkShaderModule _ShaderModule);

};

#include "VulkanPipeline.inl"