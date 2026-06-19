#include "ShaderLoader.h"

template<typename T>
Shader* ShaderLoader::LoadVertexFragmentShader(std::string _ShaderName, const char* _VertexShaderFilename, const char* _FragmentShaderFilename)
{
	Shader* shader = new Shader(_ShaderName);
	std::vector<char> vertexShaderCode;
	std::vector<char> fragmentShaderCode;
	VkShaderModule vertShaderModule{};
	VkShaderModule fragShaderModule{};
	std::vector<VkPipelineShaderStageCreateInfo> shaderStagesCreateInfo;

	shaderStagesCreateInfo.push_back(CreateShaderProgram(_VertexShaderFilename, ShaderType::VERTEX_SHADER, vertexShaderCode, vertShaderModule));
	shaderStagesCreateInfo.push_back(CreateShaderProgram(_FragmentShaderFilename, ShaderType::FRAGMENT_SHADER, fragmentShaderCode, fragShaderModule));

	ReflectShaderBindings(ShaderType::VERTEX_SHADER, vertexShaderCode, shader->GetLayoutBinding());
	ReflectShaderBindings(ShaderType::FRAGMENT_SHADER, fragmentShaderCode, shader->GetLayoutBinding());

	CreateDescriptorSetLayout(*shader, shader->GetLayoutBinding());

	CreateGraphicsPipeline(*shader, shaderStagesCreateInfo, Shader::DefaultPipelineConfigInfo<T>());

	GlobalFunctionLibrary::GetAssetDataManager()->AddData(shader);

	vkDestroyShaderModule(GlobalFunctionLibrary::GetVulkanDevice(), vertShaderModule, nullptr);
	vkDestroyShaderModule(GlobalFunctionLibrary::GetVulkanDevice(), fragShaderModule, nullptr);

	return shader;
}

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