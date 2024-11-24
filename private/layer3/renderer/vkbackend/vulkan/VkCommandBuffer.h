#pragma once

#include "../Vulkan.h"
#include "VkDevice.h"

class CVulkanCmdBuffer
{
public:
    void Init(CVulkanDevice& device);
    void Destroy();

    // Commands
private:
    VkCommandPool m_pool;
    VkCommandBuffer m_buffer;
};