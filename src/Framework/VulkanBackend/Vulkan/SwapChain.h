#pragma once

#include <vulkan/vulkan.h>

class VulkanImage;

// Vulkan Swap Chain. Requires VkState to be completely filled out with swapchain details
class VulkanSwapChain
{
public:
    VulkanSwapChain();
    void Destroy();

    void GetSwapChainImages(std::vector<VulkanImage>& images);
    VkSwapchainKHR GetSwapChain() {return m_SwapChain;}
private:
    VkSwapchainKHR m_SwapChain;
};