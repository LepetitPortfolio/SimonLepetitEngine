#include "Shader.h"
#include "../Core/GlobalFunctionLibrary.h"
#include "../Engine.h"

#include "../Common/Error.h"

#include "Textures/Texture.h"

Shader::Shader(ShaderSettings _ShaderSettings)
{
	m_ShaderName = _ShaderSettings.ShaderName;

	CreateDescriptorSetLayout(_ShaderSettings.Bindings);

	m_PipelineShaderInfo = _ShaderSettings.PipelineShaderInfo;

	m_VertexInputBindingDescriptions = _ShaderSettings.VertexInputBindingDescriptions;
	m_VertexInputAttributeDescriptions = _ShaderSettings.VertexInputAttributeDescriptions;

	CreatePipeline(m_PipelineShaderInfo, CreatePipelineConfigInfo());

	GlobalFunctionLibrary::GetAssetDataManager()->AddData(this);
}


Shader::~Shader()
{
	Cleanup();
}


std::string Shader::GetShaderName() const
{
	return m_ShaderName;
}


void Shader::Cleanup()
{
	VulkanDevice* vulkanDevice = GlobalFunctionLibrary::GetVulkanDevice();

	vkDestroyPipeline(vulkanDevice->GetLogicalDevice(), m_Pipeline, nullptr);
	vkDestroyPipelineLayout(vulkanDevice->GetLogicalDevice(), m_PipelineLayout, nullptr);

	vkDestroyDescriptorSetLayout(vulkanDevice->GetLogicalDevice(), m_DescriptorSetLayout, nullptr);
}

void Shader::RecreatePipeline()
{
	VulkanDevice* vulkanDevice = GlobalFunctionLibrary::GetVulkanDevice();

	vkDestroyPipeline(vulkanDevice->GetLogicalDevice(), m_Pipeline, nullptr);
	vkDestroyPipelineLayout(vulkanDevice->GetLogicalDevice(), m_PipelineLayout, nullptr);

	CreatePipeline(m_PipelineShaderInfo, CreatePipelineConfigInfo());
}

void Shader::GenertateUniformBufferDescriptorSetLayout(const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, VkBuffer _UniformBuffers, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets)
{
	if (_UniformBuffers)
	{
		VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo{};
		bufferInfo->buffer = _UniformBuffers;
		bufferInfo->offset = 0;
		bufferInfo->range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet writeDescriptorSet{};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = _DescriptorSet;
		writeDescriptorSet.dstBinding = _LayoutBinding.binding;
		writeDescriptorSet.dstArrayElement = 0;
		writeDescriptorSet.descriptorType = _LayoutBinding.descriptorType;
		writeDescriptorSet.descriptorCount = _LayoutBinding.descriptorCount;
		writeDescriptorSet.pBufferInfo = bufferInfo;

		_WriteDescriptorSets.push_back(writeDescriptorSet);
	}
}

void Shader::GenertateCombinedImageSamplerDescriptorSetLayout(const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, Texture* _Texture, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets)
{
	if (_Texture)
	{
		VkDescriptorImageInfo* imageInfo = new VkDescriptorImageInfo{};
		imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo->imageView = _Texture->GetTextureImageView();
		imageInfo->sampler = _Texture->GetTextureSampler();

		VkWriteDescriptorSet writeDescriptorSet{};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = _DescriptorSet;
		writeDescriptorSet.dstBinding = _LayoutBinding.binding;
		writeDescriptorSet.dstArrayElement = 0;
		writeDescriptorSet.descriptorType = _LayoutBinding.descriptorType;
		writeDescriptorSet.descriptorCount = _LayoutBinding.descriptorCount;
		writeDescriptorSet.pImageInfo = imageInfo;

		_WriteDescriptorSets.push_back(writeDescriptorSet);
	}
}

void Shader::GenertateStorageBufferDescriptorSetLayout(const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets)
{
	/*VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = ;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof();

	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = _DescriptorSet;
	writeDescriptorSet.dstBinding = _LayoutBinding.binding;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorType = _LayoutBinding.descriptorType;
	writeDescriptorSet.descriptorCount = _LayoutBinding.descriptorCount;
	writeDescriptorSet.pBufferInfo = &bufferInfo;

	_WriteDescriptorSets.push_back(writeDescriptorSet);*/
}

void Shader::CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& _Bindings)
{
	m_LayoutBinding.insert(m_LayoutBinding.end(), _Bindings.begin(), _Bindings.end());

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(_Bindings.size());
	descriptorSetLayoutCreateInfo.pBindings = _Bindings.data();

	if (vkCreateDescriptorSetLayout(GlobalFunctionLibrary::GetVulkanDevice()->GetLogicalDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
	{
		Err() << "failed to create descriptor set layout!" << std::endl;
	}
}

void Shader::CreatePipeline(const std::vector<VkPipelineShaderStageCreateInfo>& _ShaderStagesCreateInfo, const PipelineConfigInfo& _ConfigInfo)
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

	if (vkCreateGraphicsPipelines(GlobalFunctionLibrary::GetVulkanDevice()->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
	{
		Err() << "failed to create graphics pipeline!" << std::endl;
	}
}

PipelineConfigInfo Shader::CreatePipelineConfigInfo()
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
