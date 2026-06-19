#pragma once
#define GLFW_INCLUDE_VULKAN
#include "Shader.h"

#include "../Core/IVulkanDescription.h"

#include <vector>
#include <variant>

using ShaderInputType = std::variant<IVulkanDescription>;

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

class ShaderLoader
{
public :

	template<typename T>
	static Shader* LoadVertexFragmentShader(std::string _ShaderName, const char* _VertexShaderFilename, const char* _FragmentShaderFilename);

	template<typename T>
	static Shader* LoadComputeShader(std::string _ShaderName, const char* _ComputeShaderFilename);

private:

	/**
	* Crée un layout de set de descripteurs, qui définit comment les ressources (buffers, textures) sont accessibles dans les shaders.
	* Configure deux bindings : un pour les buffers uniformes (accès en vertex shader) et un pour les échantillonneurs de texture (accès en fragment shader).
	*/
	static void CreateDescriptorSetLayout(Shader& _Shader, std::vector<VkDescriptorSetLayoutBinding>* _Bindings);

	static void CreateComputeDescriptorSetLayout(Shader& _Shader, std::vector<VkDescriptorSetLayoutBinding>* _Bindings);

	/**
	* Crée le pipeline graphique, qui définit comment les vertex sont transformés en pixels à l'écran.
	* Charge les shaders (vertex et fragment), configure l'assemblage des primitives, le viewport, le rasterization,
	* le multisampling, la profondeur/stencil, le blending des couleurs, et le layout du pipeline.
	* Les shaders sont chargés depuis des fichiers SPIR-V.
	*/
	static VkPipelineShaderStageCreateInfo CreateShaderProgram(const char* _ShaderFilename, ShaderType _ShaderType, std::vector<char>& _OutShaderCode, VkShaderModule _OutShaderModule);

	/**
	* Crée le pipeline graphique, qui définit comment les vertex sont transformés en pixels à l'écran.
	* Charge les shaders (vertex et fragment), configure l'assemblage des primitives, le viewport, le rasterization,
	* le multisampling, la profondeur/stencil, le blending des couleurs, et le layout du pipeline.
	* Les shaders sont chargés depuis des fichiers SPIR-V.
	*/
	static void CreateGraphicsPipeline(Shader& _Shader, const std::vector<VkPipelineShaderStageCreateInfo>& _ShaderStagesCreateInfo, const PipelineConfigInfo& _ConfigInfo);

	template<typename T>
	static void CreateComputePipeline(Shader& _Shader, const std::vector<VkPipelineShaderStageCreateInfo>& _ShaderStagesCreateInfo);

	/**
	* Crée un module de shader à partir du code SPIR-V.
	* Les modules de shader sont utilisés pour charger les shaders dans le pipeline graphique.
	*/
	static VkShaderModule CreateShaderModule(const std::vector<char>& _ShaderCode);

	/**
	* Analyse un shader SPIR-V et affiche ses bindings (sets, bindings, types).
	* _ShaderCode : Code binaire du shader au format SPIR-V.
	*/
	static void ReflectShaderBindings(ShaderType _ShaderType, const std::vector<char>& _ShaderCode, std::vector<VkDescriptorSetLayoutBinding>* _Bindings);
};

#include "ShaderLoader.inl"