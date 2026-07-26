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