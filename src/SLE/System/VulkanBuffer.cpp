#include "VulkanBuffer.h"
#include "../Core/GlobalFunctionLibrary.h"

#include <cassert>
#include <cstring>

VulkanBuffer::VulkanBuffer(VkDeviceSize _InstanceSize, uint32_t _InstanceCount, VkBufferUsageFlags _UsageFlags, VkMemoryPropertyFlags _MemoryPropertyFlags, VkDeviceSize _MinOffsetAlignment) 
	: m_InstanceSize{ _InstanceSize }, m_InstanceCount{ _InstanceCount }, m_UsageFlags{ _UsageFlags }, m_MemoryPropertyFlags{ _MemoryPropertyFlags }
{

	m_AlignmentSize = GetAlignment(m_InstanceCount, _MinOffsetAlignment);
	m_BufferSize = m_AlignmentSize * m_InstanceCount;
	GlobalFunctionLibrary::GetVulkanPlatform()->CreateBuffer(m_BufferSize, m_UsageFlags, m_MemoryPropertyFlags, m_Buffer, m_Memory);
	m_IsInitialized = true;
}

VulkanBuffer::~VulkanBuffer()
{
	if (m_IsInitialized)
	{
		Unmap();
		vkDestroyBuffer(GlobalFunctionLibrary::GetVulkanDevice(), m_Buffer, nullptr);
		vkFreeMemory(GlobalFunctionLibrary::GetVulkanDevice(), m_Memory, nullptr);
		m_IsInitialized = false;
	}
}

VkResult VulkanBuffer::Map(VkDeviceSize _Size, VkDeviceSize _Offset)
{
	assert(m_Buffer && m_Memory && "Called map on buffer before create");
	return vkMapMemory(GlobalFunctionLibrary::GetVulkanDevice(), m_Memory, _Offset, _Size, 0, &m_Mapped);
}

void VulkanBuffer::Unmap()
{
	if (m_Mapped)
	{
		vkUnmapMemory(GlobalFunctionLibrary::GetVulkanDevice(), m_Memory);
		m_Mapped = nullptr;
	}
}

void VulkanBuffer::WriteToBuffer(void* _Data, VkDeviceSize _Size, VkDeviceSize _Offset)
{
	assert(m_Mapped && "Cannot copy to unmapped buffer");

	if (_Size == VK_WHOLE_SIZE)
	{
		memcpy(m_Mapped, _Data, m_BufferSize);
	}
	else 
	{
		char* memOffset = (char*)m_Mapped;
		memOffset += _Offset;
		memcpy(memOffset, _Data, _Size);
	}
}

VkResult VulkanBuffer::Flush(VkDeviceSize _Size, VkDeviceSize _Offset)
{
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = m_Memory;
	mappedRange.offset = _Offset;
	mappedRange.size = _Size;
	return vkFlushMappedMemoryRanges(GlobalFunctionLibrary::GetVulkanDevice(), 1, &mappedRange);
}

VkDescriptorBufferInfo VulkanBuffer::DescriptorInfo(VkDeviceSize _Size, VkDeviceSize _Offset)
{
	return VkDescriptorBufferInfo{ m_Buffer, _Offset, _Size };
}

VkResult VulkanBuffer::Invalidate(VkDeviceSize _Size, VkDeviceSize _Offset)
{
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = m_Memory;
	mappedRange.offset = _Offset;
	mappedRange.size = _Size;
	return vkInvalidateMappedMemoryRanges(GlobalFunctionLibrary::GetVulkanDevice(), 1, &mappedRange);
}

void VulkanBuffer::WriteToIndex(void* _Data, int _Index)
{
	WriteToBuffer(_Data, m_InstanceSize, _Index * m_AlignmentSize);
}

VkResult VulkanBuffer::FlushIndex(int _Index)
{
	return Flush(m_AlignmentSize, _Index * m_AlignmentSize);
}

VkDescriptorBufferInfo VulkanBuffer::DescriptorInfoForIndex(int _Index)
{
	return  DescriptorInfo(m_AlignmentSize, _Index * m_AlignmentSize);
}

VkResult VulkanBuffer::InvalidateIndex(int _Index)
{
	return Invalidate(m_AlignmentSize, _Index * m_AlignmentSize);
}

VkDeviceSize VulkanBuffer::GetAlignment(VkDeviceSize _InstanceSize, VkDeviceSize _MinOffsetAlignment)
{
	if (_MinOffsetAlignment > 0)
	{
		return (_InstanceSize + _MinOffsetAlignment - 1) & ~(_MinOffsetAlignment - 1);
	}
	return _InstanceSize;
}
