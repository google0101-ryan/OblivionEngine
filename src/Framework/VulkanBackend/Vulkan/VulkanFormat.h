#pragma once

#include <Framework/RenderBackend/RenderFormat.h>
#include <Util/Logger.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

inline VkFormat FormatToVulkan(ColorFormat format)
{
    switch (format)
    {
    case CF_BGRA8_SRGB:
        return VK_FORMAT_B8G8R8A8_SRGB;
    case CF_BGRA8_UNORM:
        return VK_FORMAT_B8G8R8A8_UNORM;
    case CF_D24_S8:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    case CF_RGBA8_SRGB:
        return VK_FORMAT_R8G8B8A8_SRGB;
    default:
        Logger::Log(LogLevel::FATAL, "Unsupported color format %d!", (int)format);
    }
}

inline ColorFormat VulkanToFormat(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_B8G8R8A8_SRGB:
        return CF_BGRA8_SRGB;
    case VK_FORMAT_B8G8R8A8_UNORM:
        return CF_BGRA8_UNORM;
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return CF_D24_S8;
    default:
        Logger::Log(LogLevel::FATAL, "Unsupported vulkan color format %d!", (int)format);
    }

    return (ColorFormat)-1;
}

inline VkColorSpaceKHR ColorSpaceToVulkan(ColorSpace colorSpace)
{
    switch (colorSpace)
    {
    case CS_SRGB_NONLINEAR:
        return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    default:
        Logger::Log(LogLevel::FATAL, "Unsupported color space %d!", (int)colorSpace);
    }
}

inline ColorSpace VulkanToColorSpace(VkColorSpaceKHR colorSpace)
{
    switch (colorSpace)
    {
    case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
        return CS_SRGB_NONLINEAR;
    default:
        Logger::Log(LogLevel::FATAL, "Unsupported vulkan color space %d!", (int)colorSpace);
    }

    return (ColorSpace)-1;
}