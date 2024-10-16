#include "Buffer.h"

extern VmaAllocator allocator;

void VulkanBuffer::Create(int size, VkBufferUsageFlags usage, bool hostVisible, bool map)
{
    VkBufferCreateInfo bufInfo = {};
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.size = size;
    bufInfo.usage = usage;

    VmaAllocationCreateInfo vmaAllocInfo = {};
    vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    if (hostVisible)
        vmaAllocInfo.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    if (map)
        vmaAllocInfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
    
    vmaCreateBuffer(allocator, &bufInfo, &vmaAllocInfo, &m_Buffer, &m_Alloc, &m_AllocInfo);

    if (map)
        m_pMappedData = m_AllocInfo.pMappedData;
    else
        m_pMappedData = nullptr;
}

void VulkanBuffer::Destroy()
{
    vmaDestroyBuffer(allocator, m_Buffer, m_Alloc);
}
