#pragma once

#include <vulkan/vulkan.h>

#include <Framework/VulkanBackend/Vulkan/CommandBuffer.h>
#include <Framework/VulkanBackend/Vulkan/VkGodStruct.h>
#include <Framework/VulkanBackend/vma.h>
#include <Framework/RenderBackend/Geometry.h>

struct StagingBuffer
{
    StagingBuffer()
    : m_bSubmitted(false),
      m_Buffer(VK_NULL_HANDLE),
      m_iOffset( 0 ),
      m_pData( nullptr) {}

    bool m_bSubmitted;
    CommandBuffer m_CmdBuffer;
    VkBuffer m_Buffer;
    VkFence m_Fence;
    VmaAllocation alloc;
    VmaAllocationInfo allocInfo;
    VkDeviceSize m_iOffset;
    uint8_t* m_pData;
};

struct CacheInfo
{
    uint64_t m_iOffset;
    uint64_t m_iSize;
    uint64_t m_iFrame;
};

class StagingManager
{
public:
    void Create();
    void Destroy();

    void BeginFrame();

    vertCacheHandle_t AllocCache(void* pData, int size);
    void Flush(VkBuffer dst);

    CacheInfo GetInfo(vertCacheHandle_t handle);
private:
    int m_iMaxBufferSize;
    VkDeviceMemory m_Memory;

    StagingBuffer m_Buffers[MAX_FRAMES];
};

extern StagingManager g_vertexStagingManager;
extern StagingManager g_indexStagingManager;