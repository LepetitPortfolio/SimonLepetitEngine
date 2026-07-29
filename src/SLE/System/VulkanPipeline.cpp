#include "VulkanPipeline.h"

#include "../Core/GlobalFunctionLibrary.h"
#include "../Common/FileReader.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>

VulkanPipeline::VulkanPipeline(const std::vector<char>* _VertCode, const std::vector<char>* _FragCode, const PipelineConfigInfo& _ConfigInfo)
{
	CreateGraphicsPipeline(_VertCode, _FragCode, _ConfigInfo);
}

VulkanPipeline::~VulkanPipeline()
{

	for (auto shaderModule : m_ShaderModules)
	{
		vkDestroyShaderModule(GlobalFunctionLibrary::GetVulkanDevice(), shaderModule.second, nullptr);
	}

	m_ShaderModules.clear();

	vkDestroyPipeline(GlobalFunctionLibrary::GetVulkanDevice(), m_GraphicsPipeline, nullptr);
}

void VulkanPipeline::Bind(VkCommandBuffer _CommandBuffer)
{
	vkCmdBindPipeline(_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
}

void VulkanPipeline::EnableAlphaBlending(PipelineConfigInfo& _ConfigInfo)
{
	_ConfigInfo.ColorBlendAttachment.blendEnable = VK_TRUE;
	_ConfigInfo.ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	_ConfigInfo.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	_ConfigInfo.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	_ConfigInfo.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	_ConfigInfo.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	_ConfigInfo.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	_ConfigInfo.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

void VulkanPipeline::CreateGraphicsPipeline(const std::vector<char>* _VertCode, const std::vector<char>* _FragCode, const PipelineConfigInfo& _ConfigInfo)
{
	assert(_ConfigInfo.PipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in _ConfigInfo");
	assert(_ConfigInfo.RenderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in _ConfigInfo");

	CreateShaderModule(_VertCode, &m_ShaderModules[ShaderType::VERTEX_SHADER]);
	CreateShaderModule(_FragCode, &m_ShaderModules[ShaderType::FRAGMENT_SHADER]);

	VkPipelineShaderStageCreateInfo shaderStages[2];
	
	shaderStages[0] = CreateShaderStage(VK_SHADER_STAGE_VERTEX_BIT, m_ShaderModules[ShaderType::VERTEX_SHADER]);
	shaderStages[1] = CreateShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, m_ShaderModules[ShaderType::FRAGMENT_SHADER]);

	auto& bindingDescriptions = _ConfigInfo.BindingDescriptions;
	auto& attributeDescriptions = _ConfigInfo.AttributeDescriptions;

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &_ConfigInfo.InputAssemblyInfo;
	pipelineInfo.pViewportState = &_ConfigInfo.ViewportInfo;
	pipelineInfo.pRasterizationState = &_ConfigInfo.RasterizationInfo;
	pipelineInfo.pMultisampleState = &_ConfigInfo.MultisampleInfo;
	pipelineInfo.pColorBlendState = &_ConfigInfo.ColorBlendInfo;
	pipelineInfo.pDepthStencilState = &_ConfigInfo.DepthStencilInfo;
	pipelineInfo.pDynamicState = &_ConfigInfo.DynamicStateInfo;

	pipelineInfo.layout = _ConfigInfo.PipelineLayout;
	pipelineInfo.renderPass = _ConfigInfo.RenderPass;
	pipelineInfo.subpass = _ConfigInfo.Subpass;

	pipelineInfo.basePipelineIndex = -1;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(GlobalFunctionLibrary::GetVulkanDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline");
	}
}

void VulkanPipeline::CreateShaderModule(const std::vector<char>* _Code, VkShaderModule* _ShaderModule)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = _Code->size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(_Code->data());

	if (vkCreateShaderModule(GlobalFunctionLibrary::GetVulkanDevice(), &createInfo, nullptr, _ShaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module");
	}
}

VkPipelineShaderStageCreateInfo VulkanPipeline::CreateShaderStage(VkShaderStageFlagBits _ShaderStage, VkShaderModule _ShaderModule)
{
	VkPipelineShaderStageCreateInfo shaderStageInfo{};

	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = _ShaderStage;
	shaderStageInfo.module = _ShaderModule;
	shaderStageInfo.pName = "main";
	shaderStageInfo.flags = 0;
	shaderStageInfo.pNext = nullptr;
	shaderStageInfo.pSpecializationInfo = nullptr;

	return shaderStageInfo;
}

