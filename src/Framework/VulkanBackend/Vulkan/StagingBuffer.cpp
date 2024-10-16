#include <Framework/VulkanBackend/Vulkan/StagingBuffer.h>
#include <Framework/Cvar/Cvar.h>
#include <cstdint>

#define VMA_IMPLEMENTATION
#include <Framework/VulkanBackend/vma.h>
#include "StagingBuffer.h"

StagingManager g_vertexStagingManager, g_indexStagingManager;

VmaAllocator allocator = VK_NULL_HANDLE;

const int STATIC_VERTEX_MEMORY = 31 * 1024 * 1024;	// make sure it fits in VERTCACHE_OFFSET_MASK!

// vertCacheHandle_t packs size, offset, and frame number into 64 bits
const int VERTCACHE_STATIC = 1;					// in the static set, not the per-frame set
const int VERTCACHE_SIZE_SHIFT = 1;
const int VERTCACHE_SIZE_MASK = 0x7fffff;		// 8 megs 
const int VERTCACHE_OFFSET_SHIFT = 24;
const int VERTCACHE_OFFSET_MASK = 0x1ffffff;	// 32 megs 
const int VERTCACHE_FRAME_SHIFT = 49;
const int VERTCACHE_FRAME_MASK = 0x7fff;		// 15 bits = 32k frames to wrap around

void StagingManager::Create()
{
    if (allocator == VK_NULL_HANDLE)
    {
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        allocatorInfo.physicalDevice = g_VkState.m_Device->GetPhysDevice();
        allocatorInfo.device = g_VkState.m_Device->GetDeviceHandle();
        allocatorInfo.instance = g_VkState.m_Instance->GetHandle();

        vmaCreateAllocator(&allocatorInfo, &allocator);
    }

    m_iMaxBufferSize = STATIC_VERTEX_MEMORY;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = m_iMaxBufferSize;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < MAX_FRAMES; i++)
    {
        m_Buffers[i].m_iOffset = 0;

        vmaCreateBuffer(allocator, &bufferCreateInfo, &allocInfo, &m_Buffers[i].m_Buffer, &m_Buffers[i].alloc, &m_Buffers[i].allocInfo);
        vkCreateFence(g_VkState.m_Device->GetDeviceHandle(), &fenceInfo, nullptr, &m_Buffers[i].m_Fence);
    }

    VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    for (int i = 0; i < MAX_FRAMES; i++)
    {
        m_Buffers[i].m_CmdBuffer.Create();

        m_Buffers[i].m_pData = (uint8_t*)m_Buffers[i].allocInfo.pMappedData;
    }
}

void StagingManager::BeginFrame()
{
        m_Buffers[g_VkState.m_iFrameIndex].m_iOffset = 0;
}

void StagingManager::Destroy()
{
    for (int i = 0; i < MAX_FRAMES; i++)
    {
        vmaDestroyBuffer(allocator, m_Buffers[i].m_Buffer, m_Buffers[i].alloc);
        m_Buffers[i].m_CmdBuffer.Destroy();
    }
}

vertCacheHandle_t StagingManager::AllocCache(void *pData, int size)
{
    if (size == 0)
    {
        return 0;
    }

    uint64_t endPos = 0;
    uint64_t offset = 0;

    endPos = m_Buffers[g_VkState.m_iFrameIndex].m_iOffset + size;
    m_Buffers[g_VkState.m_iFrameIndex].m_iOffset += size;
    if (endPos > m_iMaxBufferSize)
    {
        Logger::Log(LogLevel::FATAL, "Overflow in vertex cache");
    }

    offset = endPos - size;

    printf("Adding vertex data at offset %ld, size %ld\n", offset, endPos);

    memcpy(m_Buffers[g_VkState.m_iFrameIndex].m_pData+offset, pData, size);

    return (g_VkState.m_iFrame & VERTCACHE_FRAME_MASK) << VERTCACHE_FRAME_SHIFT
            | (offset & VERTCACHE_OFFSET_MASK) << VERTCACHE_OFFSET_SHIFT
            | (size & VERTCACHE_SIZE_MASK) << VERTCACHE_SIZE_SHIFT;
}

void StagingManager::Flush(VkBuffer dst)
{
    vkWaitForFences(g_VkState.m_Device->GetDeviceHandle(), 1, &m_Buffers[g_VkState.m_iFrameIndex].m_Fence, VK_TRUE, UINT64_MAX);
    vkResetFences(g_VkState.m_Device->GetDeviceHandle(), 1, &m_Buffers[g_VkState.m_iFrameIndex].m_Fence);
    
    m_Buffers[g_VkState.m_iFrameIndex].m_CmdBuffer.Begin();
    
    m_Buffers[g_VkState.m_iFrameIndex].m_CmdBuffer.Copy(0, 0, m_iMaxBufferSize, m_Buffers[g_VkState.m_iFrameIndex].m_Buffer, dst);
    
    m_Buffers[g_VkState.m_iFrameIndex].m_CmdBuffer.End();

    m_Buffers[g_VkState.m_iFrameIndex].m_CmdBuffer.Submit(0, nullptr, 0, nullptr, nullptr, m_Buffers[g_VkState.m_iFrameIndex].m_Fence);
}

CacheInfo StagingManager::GetInfo(vertCacheHandle_t handle)
{
    CacheInfo info;

    info.m_iOffset = (handle >> VERTCACHE_OFFSET_SHIFT) & VERTCACHE_OFFSET_MASK;
    info.m_iFrame = (handle >> VERTCACHE_FRAME_SHIFT) & VERTCACHE_FRAME_MASK;
    info.m_iSize = (handle >> VERTCACHE_SIZE_SHIFT) & VERTCACHE_SIZE_MASK;

    return info;
}
