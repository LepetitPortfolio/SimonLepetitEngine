#include "ShaderLoader.h"

#include "spirv_reflect.h"

#include "../Common/FileReader.h"
#include "../Core/GlobalFunctionLibrary.h"
#include "../System/AssetDataManager.h"
#include "../System/DataManagerBase.h"

#include <unordered_map>


/*/Shader* ShaderLoader::LoadVertexFragmentShader(std::string _ShaderName, const char* _VertexShaderFilename, const char* _FragmentShaderFilename)
{
	Shader* shader = new Shader(_ShaderName);
	std::vector<char> vertexShaderCode;
	std::vector<char> fragmentShaderCode;
	VkShaderModule vertShaderModule{};
	VkShaderModule fragShaderModule{};
	std::vector<VkPipelineShaderStageCreateInfo> shaderStagesCreateInfo;
	std::vector<VkDescriptorSetLayoutBinding> bindings;

	shaderStagesCreateInfo.push_back(CreateShaderProgram(_VertexShaderFilename, ShaderType::VERTEX_SHADER, vertexShaderCode, vertShaderModule));
	shaderStagesCreateInfo.push_back(CreateShaderProgram(_FragmentShaderFilename, ShaderType::FRAGMENT_SHADER, fragmentShaderCode, fragShaderModule));

	ReflectShaderBindings(ShaderType::VERTEX_SHADER, vertexShaderCode, bindings);
	ReflectShaderBindings(ShaderType::FRAGMENT_SHADER, fragmentShaderCode, bindings);

	CreateDescriptorSetLayout(*shader, bindings);

	CreateGraphicsPipeline(*shader, shaderStagesCreateInfo, _ShaderInputType);

	GlobalFunctionLibrary::GetAssetDataManager()->GetShaderManager()->AddData(shader);

	vkDestroyShaderModule(GlobalFunctionLibrary::GetVulkanDevice(), vertShaderModule, nullptr);
	vkDestroyShaderModule(GlobalFunctionLibrary::GetVulkanDevice(), fragShaderModule, nullptr);

	return shader;
}*/

/*Shader* ShaderLoader::LoadComputeShader(std::string _ShaderName, const char* _ComputeShaderFilename)
{
	Shader* shader = new Shader(_ShaderName);
	std::vector<char> computeShaderCode;
	VkShaderModule computeShaderModule;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStagesCreateInfo;
	std::vector<VkDescriptorSetLayoutBinding> bindings;

	shaderStagesCreateInfo.push_back(CreateShaderProgram(_ComputeShaderFilename, ShaderType::COMPUTE_SHADER, computeShaderCode, computeShaderModule));
	ReflectShaderBindings(ShaderType::COMPUTE_SHADER, computeShaderCode, bindings);

	CreateComputeDescriptorSetLayout(*shader, bindings);

	CreateComputePipeline(*shader, shaderStagesCreateInfo, _ShaderInputType);

	GlobalFunctionLibrary::GetAssetDataManager()->GetShaderManager()->AddData(shader);

	vkDestroyShaderModule(GlobalFunctionLibrary::GetVulkanDevice(), computeShaderModule, nullptr);

	return shader;
}*/

void ShaderLoader::CreateDescriptorSetLayout(Shader& _Shader, std::vector<VkDescriptorSetLayoutBinding>& _Bindings)
{
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(_Bindings.size());
	descriptorSetLayoutCreateInfo.pBindings = _Bindings.data();

	if (vkCreateDescriptorSetLayout(GlobalFunctionLibrary::GetVulkanDevice(), &descriptorSetLayoutCreateInfo, nullptr, &_Shader.GetDescriptorSetLayout()) != VK_SUCCESS)
	{
		Err() << "failed to create descriptor set layout!" << std::endl;
	}
}

void ShaderLoader::CreateComputeDescriptorSetLayout(Shader& _Shader, std::vector<VkDescriptorSetLayoutBinding>& _Bindings)
{

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(_Bindings.size());
	layoutInfo.pBindings = _Bindings.data();

	if (vkCreateDescriptorSetLayout(GlobalFunctionLibrary::GetVulkanDevice(), &layoutInfo, nullptr, &_Shader.GetDescriptorSetLayout()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute descriptor set layout!");
	}
}

VkPipelineShaderStageCreateInfo ShaderLoader::CreateShaderProgram(const char* _ShaderFilename, ShaderType _ShaderType, std::vector<char>& _OutShaderCode, VkShaderModule _OutShaderModule)
{
	_OutShaderCode = FileReader::ReadBinaryFile(_ShaderFilename);

	_OutShaderModule = CreateShaderModule(_OutShaderCode);
	
	VkPipelineShaderStageCreateInfo shaderStageCreateInfo{};
	shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfo.stage = static_cast<VkShaderStageFlagBits>(_ShaderType);
	shaderStageCreateInfo.module = _OutShaderModule;
	shaderStageCreateInfo.pName = "main";
	return shaderStageCreateInfo;
}

/*/void ShaderLoader::CreateGraphicsPipeline(Shader& _Shader, const std::vector<VkPipelineShaderStageCreateInfo>& _ShaderStagesCreateInfo)
{
	
	auto bindingDescription = std::visit([](auto&& arg) { return arg.GetBindingDescription(); }, _ShaderInputType);
	auto attributeDescriptions = std::visit([](auto&& arg) { return arg.GetAttributeDescriptions(); }, _ShaderInputType);

	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
	vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
	inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
	rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationStateCreateInfo.lineWidth = 1.0f;
	rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	//rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
	multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleStateCreateInfo.rasterizationSamples = GlobalFunctionLibrary::GetVulkanData()->MSAASamples;
	multisampleStateCreateInfo.minSampleShading = 1.0f;
	multisampleStateCreateInfo.pSampleMask = nullptr;
	multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
	depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilStateCreateInfo.minDepthBounds = 0.0f;
	depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
	depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
	depthStencilStateCreateInfo.front = {};
	depthStencilStateCreateInfo.back = {};

	VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
	colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachmentState.blendEnable = VK_FALSE;
	colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
	colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendStateCreateInfo.attachmentCount = 1;
	colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
	colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &_Shader.GetDescriptorSetLayout();
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(GlobalFunctionLibrary::GetVulkanDevice(), &pipelineLayoutCreateInfo, nullptr, &_Shader.GetPipelineLayout()) != VK_SUCCESS)
	{
		Err() << "failed to create pipeline layout!" << std::endl;
	}

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(_ShaderStagesCreateInfo.size());
	graphicsPipelineCreateInfo.pStages = _ShaderStagesCreateInfo.data();
	graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
	graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
	graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
	graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
	graphicsPipelineCreateInfo.layout = _Shader.GetPipelineLayout();
	graphicsPipelineCreateInfo.renderPass = GlobalFunctionLibrary::GetVulkanData()->RenderPass;
	graphicsPipelineCreateInfo.subpass = 0;
	graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	graphicsPipelineCreateInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(GlobalFunctionLibrary::GetVulkanDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &_Shader.GetPipeline()) != VK_SUCCESS)
	{
		Err() << "failed to create graphics pipeline!" << std::endl;
	}
}*/

/*void ShaderLoader::CreateComputePipeline(Shader& _Shader, const std::vector<VkPipelineShaderStageCreateInfo>& _ShaderStagesCreateInfo)
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
}*/

VkShaderModule ShaderLoader::CreateShaderModule(const std::vector<char>& _ShaderCode)
{
	VkShaderModule shaderModule;
	VkShaderModuleCreateInfo shaderModuleCreateInfo{};
	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.codeSize = _ShaderCode.size();
	shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(_ShaderCode.data());

	if (vkCreateShaderModule(GlobalFunctionLibrary::GetVulkanDevice(), &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		Err() << "failed to create shader module!" << std::endl;
	}

	return shaderModule;
}

void ShaderLoader::ReflectShaderBindings(ShaderType _ShaderType, const std::vector<char>& _ShaderCode, std::vector<VkDescriptorSetLayoutBinding>& _Bindings)
{
	SpvReflectShaderModule module;
	SpvReflectResult result = spvReflectCreateShaderModule(	_ShaderCode.size(), reinterpret_cast<const uint32_t*>(_ShaderCode.data()), &module);

	if (result != SPV_REFLECT_RESULT_SUCCESS)
	{
		std::cerr << "Erreur : Impossible de charger le module SPIR-V pour la réflexion." << std::endl;
		return;
	}


	// Parcourir chaque descriptor set
	for (uint32_t setIndex = 0; setIndex < module.descriptor_set_count; ++setIndex)
	{
		SpvReflectDescriptorSet& descriptorSet = module.descriptor_sets[setIndex];

		// Parcourir chaque binding dans le set
		for (uint32_t bindingIndex = 0; bindingIndex < descriptorSet.binding_count; ++bindingIndex)
		{
			SpvReflectDescriptorBinding& binding = *descriptorSet.bindings[bindingIndex];

			VkDescriptorSetLayoutBinding layoutBinding{};

			layoutBinding.binding = binding.binding;
			layoutBinding.descriptorType = static_cast<VkDescriptorType>(binding.descriptor_type);
			layoutBinding.descriptorCount = binding.count;
			layoutBinding.stageFlags = static_cast<VkShaderStageFlagBits>(_ShaderType);


			_Bindings.push_back(layoutBinding);
		}
	}

	spvReflectDestroyShaderModule(&module);
}
