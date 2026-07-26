#pragma once
#define GLFW_INCLUDE_VULKAN
#include "Shader.h"

#include "../Core/IVulkanDescription.h"

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

};

#include "ShaderLoader.inl"