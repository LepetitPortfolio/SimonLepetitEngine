#include "VulkanPipeline.h"

#include "../Core/GlobalFunctionLibrary.h"
#include "../Common/FileReader.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>

VulkanPipeline::VulkanPipeline()
{
}

VulkanPipeline::~VulkanPipeline()
{
	Cleanup();
}

void VulkanPipeline::Initialize(ShaderSettings _ShaderSettings)
{
	CreateDescriptorSetLayout(_ShaderSettings.Bindings);

	m_VertexInputBindingDescriptions = std::vector<VkVertexInputBindingDescription>(_ShaderSettings.VertexInputBindingDescriptions);
	m_VertexInputAttributeDescriptions = std::vector<VkVertexInputAttributeDescription>(_ShaderSettings.VertexInputAttributeDescriptions);
	m_PipelineShaderInfo = std::vector<VkPipelineShaderStageCreateInfo>(_ShaderSettings.PipelineShaderInfo);

	PipelineConfigInfo pipelineConfigInfo = CreatePipelineConfigInfo();
	CreatePipeline(m_PipelineShaderInfo, pipelineConfigInfo);
}

void VulkanPipeline::Cleanup()
{
	VulkanDevice* vulkanDevice = GlobalFunctionLibrary::GetVulkanDevice();

	vkDestroyPipeline(vulkanDevice->GetLogicalDevice(), m_Pipeline, nullptr);
	vkDestroyPipelineLayout(vulkanDevice->GetLogicalDevice(), m_PipelineLayout, nullptr);

	vkDestroyDescriptorSetLayout(vulkanDevice->GetLogicalDevice(), m_DescriptorSetLayout, nullptr);

}

void VulkanPipeline::Recreate()
{
	VulkanDevice* vulkanDevice = GlobalFunctionLibrary::GetVulkanDevice();

	vkDestroyPipeline(vulkanDevice->GetLogicalDevice(), m_Pipeline, nullptr);
	vkDestroyPipelineLayout(vulkanDevice->GetLogicalDevice(), m_PipelineLayout, nullptr);

	PipelineConfigInfo pipelineConfigInfo = CreatePipelineConfigInfo();
	CreatePipeline(m_PipelineShaderInfo, pipelineConfigInfo);
}

void VulkanPipeline::Bind(VkCommandBuffer _CommandBuffer)
{
	vkCmdBindPipeline(_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
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

void VulkanPipeline::CreatePipeline(const std::vector<VkPipelineShaderStageCreateInfo>& _ShaderStagesCreateInfo, const PipelineConfigInfo& _ConfigInfo)
{
	VulkanDevice* vulkanDevice = GlobalFunctionLibrary::GetVulkanDevice();

	std::vector<VkVertexInputBindingDescription> bindingDescriptions = std::vector<VkVertexInputBindingDescription>(_ConfigInfo.BindingDescriptions);
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions = std::vector<VkVertexInputAttributeDescription>(_ConfigInfo.AttributeDescriptions);

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = static_cast<uint32_t>(_ShaderStagesCreateInfo.size());
	pipelineInfo.pStages = _ShaderStagesCreateInfo.data();
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

	if (vkCreateGraphicsPipelines(vulkanDevice->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline");
	}
}

void VulkanPipeline::CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& _Bindings)
{
	m_LayoutBinding = std::vector<VkDescriptorSetLayoutBinding>(_Bindings);

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(m_LayoutBinding.size());
	descriptorSetLayoutCreateInfo.pBindings = m_LayoutBinding.data();

	if (vkCreateDescriptorSetLayout(GlobalFunctionLibrary::GetVulkanDevice()->GetLogicalDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
	{
		Err() << "failed to create descriptor set layout!" << std::endl;
	}
}

PipelineConfigInfo VulkanPipeline::CreatePipelineConfigInfo()
{
	VulkanPlatform* vulkanPlatform = GlobalFunctionLibrary::GetVulkanPlatform();
	VulkanDevice* vulkanDevice = GlobalFunctionLibrary::GetVulkanDevice();
	VkExtent2D swapChainExtent = vulkanPlatform->GetVulkanSwapchain()->GetExtent();

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;

	std::vector<VkDynamicState>* dynamicStates = new std::vector<VkDynamicState>
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates->size());
	dynamicState.pDynamicStates = dynamicStates->data();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	VkResult layoutResult = vkCreatePipelineLayout(vulkanDevice->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout);

	if (layoutResult != VK_SUCCESS)
	{
		std::cout << "Failed to create pipeline layout - " << layoutResult << std::endl;
		throw std::runtime_error("failed to create pipeline layout!");
	}
	else
	{
		std::cout << "Successfully created pipeline layout - " << layoutResult << std::endl;
	}

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f;
	depthStencil.maxDepthBounds = 1.0f;
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {};
	depthStencil.back = {};

	PipelineConfigInfo configInfo{};
	configInfo.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	configInfo.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	configInfo.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	configInfo.ViewportInfo = viewportState;
	configInfo.RasterizationInfo = rasterizer;
	configInfo.MultisampleInfo = multisampling;
	configInfo.ColorBlendAttachment = colorBlendAttachment;
	configInfo.ColorBlendInfo = colorBlending;
	configInfo.DepthStencilInfo = depthStencil;
	configInfo.DynamicStateEnables = *dynamicStates;
	configInfo.DynamicStateInfo = dynamicState;
	configInfo.PipelineLayout = m_PipelineLayout;
	configInfo.RenderPass = vulkanPlatform->GetVulkanRenderer()->GetRenderPass();
	configInfo.Subpass = 0;

	configInfo.BindingDescriptions = m_VertexInputBindingDescriptions;
	configInfo.AttributeDescriptions = m_VertexInputAttributeDescriptions;

	return configInfo;
}