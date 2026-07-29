#include "VulkanDescriptorSetLayout.h"

#include "../Core/GlobalFunctionLibrary.h"

#include <cassert>
#include <stdexcept>

VulkanDescriptorSetBuilder::VulkanDescriptorSetBuilder()
{
}

VulkanDescriptorSetBuilder& VulkanDescriptorSetBuilder::AddBinding(uint32_t _Binding, VkDescriptorType _DescriptorType, VkShaderStageFlags _ShaderStageFlags, uint32_t _Count)
{
	assert(m_Bindings.count(_Binding) == 0 && "Binding already in use");
	VkDescriptorSetLayoutBinding layoutBinding{};
	layoutBinding.binding = _Binding;
	layoutBinding.descriptorType = _DescriptorType;
	layoutBinding.descriptorCount = _Count;
	layoutBinding.stageFlags = _ShaderStageFlags;
	m_Bindings[_Binding] = layoutBinding;

	return *this;
}

VulkanDescriptorSetBuilder& VulkanDescriptorSetBuilder::AddBindings(std::vector<VkDescriptorSetLayoutBinding>* _Bindings)
{
	for (const auto& binding : *_Bindings)
	{
		assert(m_Bindings.count(binding.binding) == 0 && "Binding already in use");
		m_Bindings[binding.binding] = binding;
	}

	return *this;
}

std::unique_ptr<VulkanDescriptorSetLayout> VulkanDescriptorSetBuilder::BuildUniquePtr() const
{
	return std::make_unique<VulkanDescriptorSetLayout>(m_Bindings);
}

VulkanDescriptorSetLayout VulkanDescriptorSetBuilder::Build() const
{
	return VulkanDescriptorSetLayout(m_Bindings);
}

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> _Bindings) :m_Bindings{ _Bindings }
{
	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
	for (auto& setLayoutBinding : m_Bindings)
	{
		setLayoutBindings.push_back(setLayoutBinding.second);
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
	descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
	descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

	if (vkCreateDescriptorSetLayout(GlobalFunctionLibrary::GetVulkanDevice(), &descriptorSetLayoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	vkDestroyDescriptorSetLayout(GlobalFunctionLibrary::GetVulkanDevice(), m_DescriptorSetLayout, nullptr);
} 

VkDescriptorSetLayoutBinding VulkanDescriptorSetLayout::GetBinding(int _BindingIndex) const 
{ 
	if (_BindingIndex < m_Bindings.size())
	{
		return m_Bindings.at(_BindingIndex);
	}

	return VkDescriptorSetLayoutBinding{};
}


VulkanDescriptorPoolBuilder::VulkanDescriptorPoolBuilder()
{
}

VulkanDescriptorPoolBuilder& VulkanDescriptorPoolBuilder::AddPoolSize(VkDescriptorType _DescriptorType, uint32_t _Count)
{
	m_PoolSize.push_back({ _DescriptorType , _Count });
	return *this;
}

VulkanDescriptorPoolBuilder& VulkanDescriptorPoolBuilder::AddPoolsSize(std::vector <VkDescriptorType>* _DescriptorsType, uint32_t _Count)
{
	for (const auto& descriptorType : *_DescriptorsType)
	{
		m_PoolSize.push_back({ descriptorType, _Count });
	}
	return *this;
}

VulkanDescriptorPoolBuilder& VulkanDescriptorPoolBuilder::AddPoolsSize(std::vector<VkDescriptorSetLayoutBinding>* _Bindings, uint32_t _Count)
{
	for (const auto& binding : *_Bindings)
	{
		m_PoolSize.push_back({ binding.descriptorType, _Count });
	}
	return *this;
}


VulkanDescriptorPoolBuilder& VulkanDescriptorPoolBuilder::SetPoolFlags(VkDescriptorPoolCreateFlags _Flags)
{
	m_PoolFlags = _Flags;
	return *this;
}

VulkanDescriptorPoolBuilder& VulkanDescriptorPoolBuilder::SetMaxSets(uint32_t _Count)
{
	m_MaxSets = _Count;
	return *this;
}

std::unique_ptr<VulkanDescriptorPool> VulkanDescriptorPoolBuilder::BuildUniquePtr() const
{
	return std::make_unique<VulkanDescriptorPool>(m_MaxSets, m_PoolFlags, m_PoolSize);
}

VulkanDescriptorPool VulkanDescriptorPoolBuilder::Build() const
{
	return VulkanDescriptorPool(m_MaxSets, m_PoolFlags, m_PoolSize);
}

VulkanDescriptorPool::VulkanDescriptorPool(uint32_t _MaxSets, VkDescriptorPoolCreateFlags _PoolFlags, const std::vector<VkDescriptorPoolSize>& _PoolSizes)
{

	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(_PoolSizes.size());
	descriptorPoolInfo.pPoolSizes = _PoolSizes.data();
	descriptorPoolInfo.maxSets = _MaxSets;
	descriptorPoolInfo.flags = _PoolFlags;

	if (vkCreateDescriptorPool(GlobalFunctionLibrary::GetVulkanDevice(), &descriptorPoolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	vkDestroyDescriptorPool(GlobalFunctionLibrary::GetVulkanDevice(), m_DescriptorPool, nullptr);
}

bool VulkanDescriptorPool::AllocateDescriptor(const VkDescriptorSetLayout _DescriptorSetLayout, VkDescriptorSet& _Descriptor) const
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_DescriptorPool;
	allocInfo.pSetLayouts = &_DescriptorSetLayout;
	allocInfo.descriptorSetCount = 1;

	if (vkAllocateDescriptorSets(GlobalFunctionLibrary::GetVulkanDevice(), &allocInfo, &_Descriptor) != VK_SUCCESS)
	{
		return false;
	}
	return true;
}

void VulkanDescriptorPool::FreeDescriptor(std::vector<VkDescriptorSet>& _Descriptors) const
{
	vkFreeDescriptorSets( GlobalFunctionLibrary::GetVulkanDevice(), m_DescriptorPool,	 static_cast<uint32_t>(_Descriptors.size()), _Descriptors.data());
}

void VulkanDescriptorPool::ResetPool()
{
	vkResetDescriptorPool(GlobalFunctionLibrary::GetVulkanDevice(), m_DescriptorPool, 0);
}


VulkanDescriptorWriter::VulkanDescriptorWriter(VulkanDescriptorSetLayout* _SetLayout, VulkanDescriptorPool* _Pool)
{
	std::cout << "Creating VulkanDescriptorWriter" << std::endl;

	m_SetLayout = _SetLayout;
	m_Pool = _Pool;
}

VulkanDescriptorWriter::~VulkanDescriptorWriter()
{
	m_SetLayout = nullptr;
	m_Pool = nullptr;

	m_Writes.clear();
	
	std::cout << "Destroying VulkanDescriptorWriter" << std::endl;
}

VulkanDescriptorWriter& VulkanDescriptorWriter::WriteBuffer(uint32_t _Binding, VkDescriptorBufferInfo* _BufferInfo)
{
	assert(m_SetLayout->GetBindings().count(_Binding) == 1 && "Layout does not contain specified binding");

	auto& bindingDescription = m_SetLayout->GetBinding(_Binding);

	assert( bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType = bindingDescription.descriptorType;
	write.dstBinding = _Binding;
	write.pBufferInfo = _BufferInfo;
	write.descriptorCount = 1;

	m_Writes.push_back(write);
	return *this;
}

VulkanDescriptorWriter& VulkanDescriptorWriter::WriteImage(uint32_t _Binding, VkDescriptorImageInfo* _ImageInfo)
{
	assert(m_SetLayout->GetBindings().count(_Binding) == 1 && "Layout does not contain specified binding");

	auto& bindingDescription = m_SetLayout->GetBindings()[_Binding];

	assert( bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType = bindingDescription.descriptorType;
	write.dstBinding = _Binding;
	write.pImageInfo = _ImageInfo;
	write.descriptorCount = 1;

	m_Writes.push_back(write);
	return *this;
}

bool VulkanDescriptorWriter::Build(VkDescriptorSet& _DescriptorSet)
{
	bool success = m_Pool->AllocateDescriptor(m_SetLayout->GetDescriptorSetLayout(), _DescriptorSet);
	if (!success) 
	{
		return false;
	}

	Overwrite(_DescriptorSet);
	
	return true;
}

void VulkanDescriptorWriter::Overwrite(VkDescriptorSet& _DescriptorSet)
{
	for (auto& write : m_Writes) 
	{
		write.dstSet = _DescriptorSet;
	}
	vkUpdateDescriptorSets(GlobalFunctionLibrary::GetVulkanDevice(), m_Writes.size(), m_Writes.data(), 0, nullptr);
}
