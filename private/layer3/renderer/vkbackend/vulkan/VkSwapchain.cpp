#include "VkSwapchain.h"

void CVulkanSwapchain::Init(CVulkanInstance &instance, CVulkanDevice &device, IWindow* pWindow, VkSurfaceKHR surface)
{
    uint32_t graphicsQueue = device.GetGraphicsQueueIdx();
    uint32_t presentQueue = device.GetPresentQueueIdx();

    uint32_t queueFamilies[] = { graphicsQueue, presentQueue };
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = FRAMES_IN_FLIGHT;
    createInfo.imageFormat = device.GetSwapFormat(surface).format;
    createInfo.imageColorSpace = device.GetSwapFormat(surface).colorSpace;
    createInfo.imageExtent = VkExtent2D{ .width = pWindow->GetWidth(), .height = pWindow->GetHeight() };
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    createInfo.imageSharingMode = (graphicsQueue == presentQueue) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = (graphicsQueue == presentQueue) ? 1 : 2;
    if (createInfo.imageSharingMode == VK_SHARING_MODE_CONCURRENT)
        createInfo.pQueueFamilyIndices = queueFamilies;
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = device.GetSwapMode(surface);
    createInfo.clipped = VK_TRUE;

    CHECK_VK(vkCreateSwapchainKHR(device.GetLogicalDevice(), &createInfo, nullptr, &m_swapChain));
}

void CVulkanSwapchain::Destroy(CVulkanDevice& device)
{
    vkDestroySwapchainKHR(device.GetLogicalDevice(), m_swapChain, nullptr);
}
