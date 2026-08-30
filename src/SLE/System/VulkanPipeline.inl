#include "VulkanPipeline.h"
#include "../Core/GlobalFunctionLibrary.h"

template<typename T>
void VulkanPipeline::CreatePipelineConfigInfo(PipelineConfigInfo& _ConfigInfo)
{
	_ConfigInfo.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	_ConfigInfo.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	_ConfigInfo.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	_ConfigInfo.ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	_ConfigInfo.ViewportInfo.viewportCount = 1;
	_ConfigInfo.ViewportInfo.pViewports = nullptr;
	_ConfigInfo.ViewportInfo.scissorCount = 1;
	_ConfigInfo.ViewportInfo.pScissors = nullptr;

	_ConfigInfo.RasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	_ConfigInfo.RasterizationInfo.depthClampEnable = VK_FALSE;
	_ConfigInfo.RasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	_ConfigInfo.RasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
	_ConfigInfo.RasterizationInfo.lineWidth = 1.0f;
	_ConfigInfo.RasterizationInfo.cullMode = VK_CULL_MODE_NONE;
	_ConfigInfo.RasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	_ConfigInfo.RasterizationInfo.depthBiasEnable = VK_FALSE;
	_ConfigInfo.RasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
	_ConfigInfo.RasterizationInfo.depthBiasClamp = 0.0f;           // Optional
	_ConfigInfo.RasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

	_ConfigInfo.MultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	_ConfigInfo.MultisampleInfo.sampleShadingEnable = VK_FALSE;
	_ConfigInfo.MultisampleInfo.rasterizationSamples = GlobalFunctionLibrary::GetVulkanData()->MSAASamples;
	_ConfigInfo.MultisampleInfo.minSampleShading = 1.0f;           // Optional
	_ConfigInfo.MultisampleInfo.pSampleMask = nullptr;             // Optional
	_ConfigInfo.MultisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
	_ConfigInfo.MultisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

	_ConfigInfo.ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	_ConfigInfo.ColorBlendAttachment.blendEnable = VK_FALSE;
	_ConfigInfo.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
	_ConfigInfo.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
	_ConfigInfo.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
	_ConfigInfo.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
	_ConfigInfo.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
	_ConfigInfo.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

	_ConfigInfo.ColorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	_ConfigInfo.ColorBlendInfo.logicOpEnable = VK_FALSE;
	_ConfigInfo.ColorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
	_ConfigInfo.ColorBlendInfo.attachmentCount = 1;
	_ConfigInfo.ColorBlendInfo.pAttachments = &_ConfigInfo.ColorBlendAttachment;
	_ConfigInfo.ColorBlendInfo.blendConstants[0] = 0.0f;  // Optional
	_ConfigInfo.ColorBlendInfo.blendConstants[1] = 0.0f;  // Optional
	_ConfigInfo.ColorBlendInfo.blendConstants[2] = 0.0f;  // Optional
	_ConfigInfo.ColorBlendInfo.blendConstants[3] = 0.0f;  // Optional

	_ConfigInfo.DepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	_ConfigInfo.DepthStencilInfo.depthTestEnable = VK_TRUE;
	_ConfigInfo.DepthStencilInfo.depthWriteEnable = VK_TRUE;
	_ConfigInfo.DepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	_ConfigInfo.DepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	_ConfigInfo.DepthStencilInfo.minDepthBounds = 0.0f;  // Optional
	_ConfigInfo.DepthStencilInfo.maxDepthBounds = 1.0f;  // Optional
	_ConfigInfo.DepthStencilInfo.stencilTestEnable = VK_FALSE;
	_ConfigInfo.DepthStencilInfo.front = {};  // Optional
	_ConfigInfo.DepthStencilInfo.back = {};   // Optional

	_ConfigInfo.DynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	_ConfigInfo.DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	_ConfigInfo.DynamicStateInfo.pDynamicStates = _ConfigInfo.DynamicStateEnables.data();
	_ConfigInfo.DynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(_ConfigInfo.DynamicStateEnables.size());
	_ConfigInfo.DynamicStateInfo.flags = 0;

	_ConfigInfo.BindingDescriptions = T::GetBindingDescription();
	_ConfigInfo.AttributeDescriptions = T::GetAttributeDescriptions();

}