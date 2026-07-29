#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanPlatform.h"

#include <memory>
#include <unordered_map>
#include <vector>


class VulkanDescriptorSetLayout
{
public:
	VulkanDescriptorSetLayout(std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> _Bindings);
	~VulkanDescriptorSetLayout();

	VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
	VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;

	VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }
	std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> GetBindings() const { return m_Bindings; }
	VkDescriptorSetLayoutBinding GetBinding(int _BindingIndex) const;

private:

	VkDescriptorSetLayout m_DescriptorSetLayout;
	std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings;

	friend class VulkanDescriptorWriter;
};

class VulkanDescriptorSetBuilder
{
public:
	VulkanDescriptorSetBuilder();
	VulkanDescriptorSetBuilder& AddBinding(uint32_t _Binding, VkDescriptorType _DescriptorType, VkShaderStageFlags _ShaderStageFlags, uint32_t _Count = 1);
	VulkanDescriptorSetBuilder& AddBindings(std::vector<VkDescriptorSetLayoutBinding>* _Bindings);

	std::unique_ptr<VulkanDescriptorSetLayout> BuildUniquePtr() const;
	VulkanDescriptorSetLayout Build() const;

private:

	std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings{};
};

class VulkanDescriptorPool
{
public:
	VulkanDescriptorPool(uint32_t _MaxSets, VkDescriptorPoolCreateFlags _PoolFlags, const std::vector<VkDescriptorPoolSize>& _PoolSizes);
	~VulkanDescriptorPool();

	VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
	VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;

	bool AllocateDescriptor(const VkDescriptorSetLayout _DescriptorSetLayout, VkDescriptorSet& _Descriptor) const;
	void FreeDescriptor(std::vector<VkDescriptorSet>& _Descriptors) const;
	void ResetPool();

private:

	VkDescriptorPool m_DescriptorPool;

	friend class VulkanDescriptorWriter;
};

class VulkanDescriptorPoolBuilder
{

public:
	VulkanDescriptorPoolBuilder();

	VulkanDescriptorPoolBuilder& AddPoolSize(VkDescriptorType _DescriptorType, uint32_t _Count);
	VulkanDescriptorPoolBuilder& AddPoolsSize(std::vector <VkDescriptorType>* _DescriptorsType, uint32_t _Count);
	VulkanDescriptorPoolBuilder& AddPoolsSize(std::vector<VkDescriptorSetLayoutBinding>* _Bindings, uint32_t _Count);
	VulkanDescriptorPoolBuilder& SetPoolFlags(VkDescriptorPoolCreateFlags _Flags);
	VulkanDescriptorPoolBuilder& SetMaxSets(uint32_t _Count);

	std::unique_ptr<VulkanDescriptorPool> BuildUniquePtr() const;
	VulkanDescriptorPool Build() const;

private:

	std::vector<VkDescriptorPoolSize> m_PoolSize{};
	uint32_t m_MaxSets = 1000;
	VkDescriptorPoolCreateFlags m_PoolFlags = 0;
};

class VulkanDescriptorWriter
{

public:
	VulkanDescriptorWriter(VulkanDescriptorSetLayout* _SetLayout, VulkanDescriptorPool* _Pool);
	~VulkanDescriptorWriter();

	VulkanDescriptorWriter& WriteBuffer(uint32_t _Binding, VkDescriptorBufferInfo* _BufferInfo);
	VulkanDescriptorWriter& WriteImage(uint32_t _Binding, VkDescriptorImageInfo* _ImageInfo);

	bool Build(VkDescriptorSet& _DescriptorSet);
	void Overwrite(VkDescriptorSet& _DescriptorSet);

private:
	VulkanDescriptorSetLayout* m_SetLayout;
	VulkanDescriptorPool* m_Pool;
	std::vector<VkWriteDescriptorSet> m_Writes;
};