#pragma once
#include <vulkan/vulkan.h>
#include "Shader.h"

#include "../Core/IVulkanDescription.h"
#include "../System/VulkanPipeline.h"

#include <vector>

class ShaderLoader
{
public :

	template<typename T>
	static Shader* LoadVertexFragmentShader(std::string _ShaderName, const char* _VertexShaderFilename, const char* _FragmentShaderFilename);

	//template<typename T>
	//static Shader* LoadComputeShader(std::string _ShaderName, const char* _ComputeShaderFilename);

private:

	static void ReflectShaderBindings(ShaderType _ShaderType, const std::vector<char>& _ShaderCode, std::vector<VkDescriptorSetLayoutBinding>* _Bindings);

	/**
	* Crée le pipeline graphique, qui définit comment les vertex sont transformés en pixels à l'écran.
	* Charge les shaders (vertex et fragment), configure l'assemblage des primitives, le viewport, le rasterization,
	* le multisampling, la profondeur/stencil, le blending des couleurs, et le layout du pipeline.
	* Les shaders sont chargés depuis des fichiers SPIR-V.
	*/
	static VkPipelineShaderStageCreateInfo CreateShaderProgram(ShaderType _ShaderType, std::vector<char>& _ShaderCode, VkShaderModule _OutShaderModule);

	/**
	* Crée un module de shader à partir du code SPIR-V.
	* Les modules de shader sont utilis�s pour charger les shaders dans le pipeline graphique.
	*/
	static VkShaderModule CreateShaderModule(const std::vector<char>& _ShaderCode);
};

#include "ShaderLoader.inl"