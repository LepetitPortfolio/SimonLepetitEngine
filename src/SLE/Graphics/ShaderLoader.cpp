#include "ShaderLoader.h"

#include "spirv_reflect.h"

#include "../Common/FileReader.h"
#include "../Core/GlobalFunctionLibrary.h"
#include "../System/AssetDataManager.h"
#include "../System/AssetDataManagerBase.h"

#include <unordered_map>

void ShaderLoader::ReflectShaderBindings(ShaderType _ShaderType, const std::vector<char>& _ShaderCode, std::vector<VkDescriptorSetLayoutBinding>* _Bindings)
{
	SpvReflectShaderModule module;
	SpvReflectResult result = spvReflectCreateShaderModule(_ShaderCode.size(), reinterpret_cast<const uint32_t*>(_ShaderCode.data()), &module);

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

VkPipelineShaderStageCreateInfo ShaderLoader::CreateShaderProgram(ShaderType _ShaderType, std::vector<char>& _ShaderCode, VkShaderModule _OutShaderModule)
{
	_OutShaderModule = CreateShaderModule(_ShaderCode);

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

	if (vkCreateShaderModule(GlobalFunctionLibrary::GetVulkanDevice()->GetLogicalDevice(), &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		Err() << "failed to create shader module!" << std::endl;
	}

	return shaderModule;
}
