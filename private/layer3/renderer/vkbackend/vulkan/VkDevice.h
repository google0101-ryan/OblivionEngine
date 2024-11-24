#pragma once

#include <vulkan/vulkan.h>
#include "VkInstance.h"
#include "VkColors.h"

class CVulkanDevice
{
public:
    void Init(CVulkanInstance& pInstance, VkSurfaceKHR surface);
    void Destroy();

    // Picks the preferred format and returns it
    VkSurfaceFormatKHR GetSwapFormat(VkSurfaceKHR surf);
    VkPresentModeKHR GetSwapMode(VkSurfaceKHR surf);

    uint32_t GetGraphicsQueueIdx() const { return m_graphicsQueue; }
    uint32_t GetPresentQueueIdx() const { return m_presentQueue; }

    VkPhysicalDevice GetDevice()
    {
        return m_device;
    }

    VkDevice GetLogicalDevice()
    {
        return m_logicalDevice;
    }
private:
    VkPhysicalDevice m_device;
    VkDevice m_logicalDevice;
    uint32_t m_graphicsQueue;
    uint32_t m_presentQueue;
};