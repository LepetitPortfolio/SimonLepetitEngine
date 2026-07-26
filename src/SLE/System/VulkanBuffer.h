#pragma once
#include "VulkanPlatform.h"

class VulkanBuffer
{
public:
    VulkanBuffer(VkDeviceSize _InstanceSize, uint32_t _InstanceCount, VkBufferUsageFlags _UsageFlags, VkMemoryPropertyFlags _MemoryPropertyFlags, VkDeviceSize _MinOffsetAlignment = 1);
    ~VulkanBuffer();

    VulkanBuffer(const VulkanBuffer&) = delete;
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;

    VkResult Map(VkDeviceSize _Size = VK_WHOLE_SIZE, VkDeviceSize _Offset = 0);
    void Unmap();

    void WriteToBuffer(void* _Data, VkDeviceSize _Size = VK_WHOLE_SIZE, VkDeviceSize _Offset = 0);
    VkResult Flush(VkDeviceSize _Size = VK_WHOLE_SIZE, VkDeviceSize _Offset = 0);
    VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize _Size = VK_WHOLE_SIZE, VkDeviceSize _Offset = 0);
    VkResult Invalidate(VkDeviceSize _Size = VK_WHOLE_SIZE, VkDeviceSize _Offset = 0);

    void WriteToIndex(void* _Data, int _Index);
    VkResult FlushIndex(int _Index);
    VkDescriptorBufferInfo DescriptorInfoForIndex(int _Index);
    VkResult InvalidateIndex(int _Index);

    VkBuffer GetBuffer() const { return m_Buffer; }
    void* GetMappedMemory() const { return m_Mapped; }
    uint32_t GetInstanceCount() const { return m_InstanceCount; }
    VkDeviceSize GetInstanceSize() const { return m_InstanceSize; }
    VkDeviceSize GetAlignmentSize() const { return m_InstanceSize; }
    VkBufferUsageFlags GetUsageFlags() const { return m_UsageFlags; }
    VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return m_MemoryPropertyFlags; }
    VkDeviceSize GetBufferSize() const { return m_BufferSize; }

private:
    static VkDeviceSize GetAlignment(VkDeviceSize _InstanceSize, VkDeviceSize _MinOffsetAlignment);

    void* m_Mapped = nullptr;
    VkBuffer m_Buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;

    VkDeviceSize m_BufferSize;
    uint32_t m_InstanceCount;
    VkDeviceSize m_InstanceSize;
    VkDeviceSize m_AlignmentSize;
    VkBufferUsageFlags m_UsageFlags;
    VkMemoryPropertyFlags m_MemoryPropertyFlags;
    bool m_IsInitialized = false;
};