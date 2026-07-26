#include "Shader.h"
#include "../Core/GlobalFunctionLibrary.h"

template<typename T>
Shader::Shader(ShaderSettings<T> _ShaderSettings)
{
	m_ShaderName = _ShaderSettings.ShaderName;

	m_DescriptorPool = _ShaderSettings.DescriptorPoolBuilder.BuildUniquePtr();

	m_DescriptorSetLayout = _ShaderSettings.DescriptorSetBuilder.BuildUniquePtr();
	m_SimpleRenderSystem = std::make_unique<VulkanSimpleRenderSystem>();
	m_SimpleRenderSystem->Init(_ShaderSettings, m_DescriptorSetLayout->GetDescriptorSetLayout());

	GlobalFunctionLibrary::GetAssetDataManager()->AddData(this);
}
