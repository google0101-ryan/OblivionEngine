#pragma once

#include <Framework/VulkanBackend/vma.h>

class VulkanBuffer
{
public:
    void Create(int size, VkBufferUsageFlags usage, bool hostVisible, bool map);
    void Destroy();

    VkBuffer GetBuffer() { return m_Buffer; }
    void* GetMappedData() {return m_pMappedData;}
private:
    VkBuffer m_Buffer;
    void* m_pMappedData;
    VmaAllocation m_Alloc;
    VmaAllocationInfo m_AllocInfo;
};