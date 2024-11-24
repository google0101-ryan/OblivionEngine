#pragma once

#include "../Vulkan.h"
#include "VkDevice.h"
#include "VkInstance.h"
#include <layer2/Window.h>

class CVulkanSwapchain
{
public:
    void Init(CVulkanInstance& instance, CVulkanDevice& device, IWindow* pWindow, VkSurfaceKHR surface);
    void Destroy(CVulkanDevice& device);
private:
    VkSwapchainKHR m_swapChain;
};