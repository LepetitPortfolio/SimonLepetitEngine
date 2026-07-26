#pragma once

#include "../Core/AssetData.h"

#include "../System/VulkanSimpleRenderSystem.h"
#include "../System/VulkanDescriptorSetLayout.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

template<typename T>
struct ShaderSettings
{
	VkShaderStageFlags ShaderStageFlags;
	std::string ShaderName;
	std::map<VkShaderStageFlagBits, std::vector<char>*> ShaderCode;

	VkRenderPass RenderPass;
	VulkanDescriptorSetBuilder DescriptorSetBuilder;
	VulkanDescriptorPoolBuilder DescriptorPoolBuilder;
};

class Shader : public AssetData
{
public:

	template<typename T>
	Shader(ShaderSettings<T> _ShaderSettings);
	~Shader();

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	std::string GetShaderName() const;

	VulkanDescriptorSetLayout* GetDescriptorSetLayout() const { return m_DescriptorSetLayout.get(); }
	VulkanDescriptorPool* GetDescriptorPool() const { return m_DescriptorPool.get(); }

	void RenderGameObjects(VkCommandBuffer _CommandBuffer, VkDescriptorSet _DescriptorSet, class Transform* _Transform, class Model* _Model);

	void GenertateDescriptorSetLayout(std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets, const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, uint32_t _FrameIndex);
	void GenertateDescriptorSetLayout(std::vector<VkWriteDescriptorSet>& _WriteDescriptorSets, const VkDescriptorSetLayoutBinding& _LayoutBinding, VkDescriptorSet _DescriptorSet, class Texture* _Texture);

	void Cleanup();

private :

	std::string m_ShaderName;

	std::unique_ptr <VulkanSimpleRenderSystem> m_SimpleRenderSystem;
	std::unique_ptr <VulkanDescriptorSetLayout> m_DescriptorSetLayout;
	std::unique_ptr <VulkanDescriptorPool> m_DescriptorPool;


};

#include "Shader.inl"
