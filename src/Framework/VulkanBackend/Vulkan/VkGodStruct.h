#pragma once

#include <vulkan/vulkan.h>

#include <Framework/VulkanBackend/Vulkan/VulkanInstance.h>
#include <Framework/VulkanBackend/Vulkan/VulkanDevice.h>
#include <Framework/VulkanBackend/Vulkan/SwapChain.h>
#include <Framework/VulkanBackend/Vulkan/Image.h>
#include <Framework/VulkanBackend/Vulkan/CommandBuffer.h>
#include <Framework/VulkanBackend/Vulkan/Buffer.h>

// Most of Vulkan relies on global state. Store it here for other components to access
// TODO: This sucks. Maybe make it not suck?

#define MAX_FRAMES 2

struct VkState
{
    VulkanInstance* m_Instance;
    VulkanDevice* m_Device;
    VkSurfaceKHR m_Surface; // One of the only times that a raw Vulkan data type will be in this struct
    DisplayFormat m_Format;
    SwapChainPresentMode m_PresentMode;
    int m_iSwapChainWidth, m_iSwapChainHeight;
    VulkanSwapChain* m_SwapChain;
    std::vector<VulkanImage> m_SwapChainImages;
    std::vector<VkFramebuffer> m_SwapChainFramebuffers;
    CommandBuffer m_FrameCommandBuffer[MAX_FRAMES];
    VkSemaphore m_imageSema[MAX_FRAMES]; // The above comment was a lie
    VkSemaphore m_renderSema[MAX_FRAMES];
    VkFence m_inFlightFence[MAX_FRAMES];
    int m_iFrameIndex;
    uint64_t m_iFrame;
    VulkanBuffer m_VertexBuffer, m_IndexBuffer;
    VulkanBuffer m_UniformBuffers[MAX_FRAMES];
    VulkanImage* depthBuffer;
};

extern VkState g_VkState;