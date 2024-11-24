#include "VkCommandBuffer.h"

void CVulkanCmdBuffer::Init(CVulkanDevice& device)
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = device.GetGraphicsQueueIdx();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    CHECK_VK(vkCreateCommandPool(device.GetLogicalDevice(), &poolInfo, nullptr, &m_pool));

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_pool;
    allocInfo.commandBufferCount = 1;

    CHECK_VK(vkAllocateCommandBuffers(device.GetLogicalDevice(), &allocInfo, &m_buffer));
}