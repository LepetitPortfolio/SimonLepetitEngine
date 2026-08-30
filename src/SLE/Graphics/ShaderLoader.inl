#include "ShaderLoader.h"
#include "../System/VulkanIncludes.h"

template<typename T>
Shader* ShaderLoader::LoadVertexFragmentShader(std::string _ShaderName, const char* _VertexShaderFilename, const char* _FragmentShaderFilename)
{
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	ShaderCodeSettings vertexShaderSettings{};
	ShaderCodeSettings fragmentShaderSettings{};

	vertexShaderSettings.ShaderCode = FileReader::ReadBinaryFile(_VertexShaderFilename);
	fragmentShaderSettings.ShaderCode = FileReader::ReadBinaryFile(_FragmentShaderFilename);

	ReflectShaderBindings(ShaderType::VERTEX_SHADER, vertexShaderSettings.ShaderCode, &bindings);
	ReflectShaderBindings(ShaderType::FRAGMENT_SHADER, fragmentShaderSettings.ShaderCode, &bindings);

	
	ShaderSettings shaderSettings{};
	shaderSettings.ShaderName = _ShaderName;
	shaderSettings.ShaderCodeInfos[VK_SHADER_STAGE_VERTEX_BIT] = vertexShaderSettings;
	shaderSettings.ShaderCodeInfos[VK_SHADER_STAGE_FRAGMENT_BIT] = fragmentShaderSettings;
	shaderSettings.Bindings = bindings;
	shaderSettings.PipelineShaderInfo.push_back(CreateShaderProgram(ShaderType::VERTEX_SHADER, vertexShaderSettings.ShaderCode, vertexShaderSettings.ShaderModule));
	shaderSettings.PipelineShaderInfo.push_back(CreateShaderProgram(ShaderType::FRAGMENT_SHADER, fragmentShaderSettings.ShaderCode, fragmentShaderSettings.ShaderModule));
	shaderSettings.ShaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderSettings.RenderPass = GlobalFunctionLibrary::GetVulkanPlatform()->GetVulkanRenderer()->GetRenderPass();
	shaderSettings.VertexInputBindingDescriptions = T::GetBindingDescription();
	shaderSettings.VertexInputAttributeDescriptions = T::GetAttributeDescriptions();

	Shader* shader = new Shader(shaderSettings);
	
//	vkDestroyShaderModule(GlobalFunctionLibrary::GetVulkanDevice(), vertShaderModule, nullptr);
//	vkDestroyShaderModule(GlobalFunctionLibrary::GetVulkanDevice(), fragShaderModule, nullptr);

	return shader;
}
/*
template<typename T>
Shader* ShaderLoader::LoadComputeShader(std::string _ShaderName, const char* _ComputeShaderFilename)
{
	Shader* shader = new Shader(_ShaderName);
	std::vector<char> computeShaderCode;
	VkShaderModule computeShaderModule;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStagesCreateInfo;
	std::vector<VkDescriptorSetLayoutBinding> bindings;

	shaderStagesCreateInfo.push_back(CreateShaderProgram(_ComputeShaderFilename, ShaderType::COMPUTE_SHADER, computeShaderCode, computeShaderModule));
	ReflectShaderBindings(ShaderType::COMPUTE_SHADER, computeShaderCode, bindings);

	CreateComputeDescriptorSetLayout(*shader, bindings);

	CreateComputePipeline<T>(*shader, shaderStagesCreateInfo);

	GlobalFunctionLibrary::GetAssetDataManager()->AddData(shader);

	vkDestroyShaderModule(GlobalFunctionLibrary::GetVulkanDevice(), computeShaderModule, nullptr);

	return shader;
}
*/
/*
template<typename T>
void ShaderLoader::CreateComputePipeline(Shader& _Shader, const std::vector<VkPipelineShaderStageCreateInfo>& _ShaderStagesCreateInfo)
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &_Shader.GetDescriptorSetLayout();
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(GlobalFunctionLibrary::GetVulkanDevice(), &pipelineLayoutInfo, nullptr, &_Shader.GetPipelineLayout()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute pipeline layout!");
	}

	VkComputePipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.layout = _Shader.GetPipelineLayout();
	pipelineInfo.stage = _ShaderStagesCreateInfo[0];

	if (vkCreateComputePipelines(GlobalFunctionLibrary::GetVulkanDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_Shader.GetPipeline()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute pipeline!");
	}
}
*/