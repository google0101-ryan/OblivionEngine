#include <Framework/VulkanBackend/Vulkan/VkGodStruct.h>

#include <Util/Logger.h>
#include "VulkanFormat.h"
#include "SwapChain.h"

VulkanSwapChain::VulkanSwapChain()
{
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = g_VkState.m_Surface;

    // Get image count
    auto caps = g_VkState.m_Device->GetSurfaceCaps(g_VkState.m_Surface);

    createInfo.minImageCount = caps.minImageCount+1;
    if(createInfo.minImageCount > caps.maxImageCount && caps.maxImageCount != 0)
        createInfo.minImageCount = caps.maxImageCount;
    createInfo.imageFormat = FormatToVulkan(g_VkState.m_Format.m_Format);
    createInfo.imageColorSpace = ColorSpaceToVulkan(g_VkState.m_Format.m_ColorSpace);
    createInfo.imageExtent = {.width = (uint32_t)g_VkState.m_iSwapChainWidth, .height = (uint32_t)g_VkState.m_iSwapChainHeight};
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t indices[2] = {g_VkState.m_Device->GetGraphicsQueueIdx(), g_VkState.m_Device->GetPresentQueueIdx()};

    if (g_VkState.m_Device->GetPresentQueueIdx() != g_VkState.m_Device->GetGraphicsQueueIdx())
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = indices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }
    createInfo.preTransform = caps.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = PresentModeToVulkan(g_VkState.m_PresentMode);
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = nullptr;

    if (vkCreateSwapchainKHR(g_VkState.m_Device->GetDeviceHandle(), &createInfo, NULL, &m_SwapChain) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to create swapchain");
}

void VulkanSwapChain::Destroy()
{
    vkDestroySwapchainKHR(g_VkState.m_Device->GetDeviceHandle(), m_SwapChain, nullptr);
}

void VulkanSwapChain::GetSwapChainImages(std::vector<VulkanImage> &images)
{
    std::vector<VkImage> vkImages;
    std::vector<VkImageView> vkImageViews;

    uint32_t swapChainCount;
    vkGetSwapchainImagesKHR(g_VkState.m_Device->GetDeviceHandle(), m_SwapChain, &swapChainCount, nullptr);
    vkImages.resize(swapChainCount);
    vkGetSwapchainImagesKHR(g_VkState.m_Device->GetDeviceHandle(), m_SwapChain, &swapChainCount, vkImages.data());

    vkImageViews.resize(swapChainCount);
    for (size_t i = 0; i < vkImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = vkImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = FormatToVulkan(g_VkState.m_Format.m_Format);
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        
        if (vkCreateImageView(g_VkState.m_Device->GetDeviceHandle(), &createInfo, nullptr, &vkImageViews[i]) != VK_SUCCESS)
            Logger::Log(LogLevel::FATAL, "Failed to create swapchain image view");
    }

    for (size_t i = 0; i < swapChainCount; i++)
    {
        VulkanImage image(vkImages[i], vkImageViews[i], g_VkState.m_iSwapChainWidth, g_VkState.m_iSwapChainHeight, g_VkState.m_Format.m_Format);
        images.push_back(image);
    }
}
