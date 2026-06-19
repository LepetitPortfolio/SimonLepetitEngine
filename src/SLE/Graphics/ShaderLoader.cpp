#include "ShaderLoader.h"

#include "spirv_reflect.h"

#include "../Common/FileReader.h"
#include "../Core/GlobalFunctionLibrary.h"
#include "../System/AssetDataManager.h"
#include "../System/DataManagerBase.h"

#include <unordered_map>

void ShaderLoader::CreateDescriptorSetLayout(Shader& _Shader, std::vector<VkDescriptorSetLayoutBinding>* _Bindings)
{
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(_Bindings->size());
	descriptorSetLayoutCreateInfo.pBindings = _Bindings->data();

	if (vkCreateDescriptorSetLayout(GlobalFunctionLibrary::GetVulkanDevice(), &descriptorSetLayoutCreateInfo, nullptr, &_Shader.GetDescriptorSetLayout()) != VK_SUCCESS)
	{
		Err() << "failed to create descriptor set layout!" << std::endl;
	}
}

void ShaderLoader::CreateComputeDescriptorSetLayout(Shader& _Shader, std::vector<VkDescriptorSetLayoutBinding>* _Bindings)
{

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(_Bindings->size());
	layoutInfo.pBindings = _Bindings->data();

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

void ShaderLoader::ReflectShaderBindings(ShaderType _ShaderType, const std::vector<char>& _ShaderCode, std::vector<VkDescriptorSetLayoutBinding>* _Bindings)
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


			_Bindings->push_back(layoutBinding);
		}
	}

	spvReflectDestroyShaderModule(&module);
}

void ShaderLoader::CreateGraphicsPipeline(Shader& _Shader, const std::vector<VkPipelineShaderStageCreateInfo>& _ShaderStagesCreateInfo, const PipelineConfigInfo& _ConfigInfo)
{
	auto& bindingDescriptions = _ConfigInfo.BindingDescriptions;
	auto& attributeDescriptions = _ConfigInfo.AttributeDescriptions;

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = static_cast<uint32_t>(_ShaderStagesCreateInfo.size());
	pipelineCreateInfo.pStages = _ShaderStagesCreateInfo.data();
	pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
	pipelineCreateInfo.pInputAssemblyState = &_ConfigInfo.InputAssemblyInfo;
	pipelineCreateInfo.pViewportState = &_ConfigInfo.ViewportInfo;
	pipelineCreateInfo.pRasterizationState = &_ConfigInfo.RasterizationInfo;
	pipelineCreateInfo.pMultisampleState = &_ConfigInfo.MultisampleInfo;
	pipelineCreateInfo.pColorBlendState = &_ConfigInfo.ColorBlendInfo;
	pipelineCreateInfo.pDepthStencilState = &_ConfigInfo.DepthStencilInfo;
	pipelineCreateInfo.pDynamicState = &_ConfigInfo.DynamicStateInfo;

	pipelineCreateInfo.layout = _ConfigInfo.PipelineLayout;
	pipelineCreateInfo.renderPass = _ConfigInfo.RenderPass;
	pipelineCreateInfo.subpass = _ConfigInfo.Subpass;
	pipelineCreateInfo.basePipelineIndex = -1;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(GlobalFunctionLibrary::GetVulkanDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &_Shader.GetPipeline()) != VK_SUCCESS)
	{
		Err() << "failed to create graphics pipeline!" << std::endl;
	}
}