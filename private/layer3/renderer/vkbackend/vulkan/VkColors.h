#pragma once

#include "../../BackendFormat.h"
#include "../Vulkan.h"

class CVulkanFormat : public SBackendFormat
{
public:
    VkFormat Convert() const
    {
        switch (value)
        {
        case FORMAT_RGBA8_SRGB:
            return VK_FORMAT_R8G8B8_SRGB;
        default:
            printf("Failed to convert format '%d' to vulkan!\n", (int)value);
            exit(1);
        }
    }
};