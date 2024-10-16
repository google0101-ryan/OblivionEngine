#include <Framework/VulkanBackend/Vulkan/CommandBuffer.h>
#include <Framework/VulkanBackend/Vulkan/VkGodStruct.h>

#include <Util/Logger.h>
#include <cstdint>
#include <vulkan/vulkan_core.h>
#include "CommandBuffer.h"

void CommandBuffer::Create()
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = g_VkState.m_Device->GetGraphicsQueueIdx();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(g_VkState.m_Device->GetDeviceHandle(), &poolInfo, nullptr, &m_CmdPool) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to create command pool");
    
    VkCommandBufferAllocateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    bufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    bufferInfo.commandBufferCount = 1;
    bufferInfo.commandPool = m_CmdPool;

    if (vkAllocateCommandBuffers(g_VkState.m_Device->GetDeviceHandle(), &bufferInfo, &m_CmdBuffer) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to allocate buffer");
}

void CommandBuffer::Destroy()
{
    vkFreeCommandBuffers(g_VkState.m_Device->GetDeviceHandle(), m_CmdPool, 1, &m_CmdBuffer);
    vkDestroyCommandPool(g_VkState.m_Device->GetDeviceHandle(), m_CmdPool, nullptr);
}

void CommandBuffer::Reset()
{
    vkResetCommandBuffer(m_CmdBuffer, 0);
}

void CommandBuffer::Submit(int waitSemaCount, VkSemaphore *pWaitSemas, int signalSemaCount, VkSemaphore *pSignalSemas, WaitStages *stages, VkFence waitFence)
{
    VkSubmitInfo submit = {};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.waitSemaphoreCount = waitSemaCount;
    submit.pWaitSemaphores = pWaitSemas;
    submit.pWaitDstStageMask = (VkPipelineStageFlags*)stages;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &m_CmdBuffer;
    submit.signalSemaphoreCount = signalSemaCount;
    submit.pSignalSemaphores = pSignalSemas;

    if (vkQueueSubmit(g_VkState.m_Device->GetGraphicsQueue(), 1, &submit, waitFence) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to submit queue");
}

void CommandBuffer::Begin()
{
    VkCommandBufferBeginInfo begin = {};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin.flags = 0;
    begin.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(m_CmdBuffer, &begin) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to begin command buffer");
}

void CommandBuffer::BeginRenderPass(IRenderPass* pPass, bool depthOnlyPass)
{
    auto pass = (VulkanRenderPass*)pPass;

    auto& images = pass->GetOutputs();
    std::vector<VkImageView> views;
    for (auto& image : images)
    {
        VulkanImage* pImage = (VulkanImage*)image->GetAPIData();
        views.push_back(pImage->GetView());
    }

    VkRenderPassAttachmentBeginInfo attachmentInfo = {};
    attachmentInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO;
    attachmentInfo.pAttachments = views.data();
    attachmentInfo.attachmentCount = views.size();

    VkRenderPassBeginInfo passInfo = {};
    VkClearValue value = {};
    passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    passInfo.pNext = &attachmentInfo;
    passInfo.renderPass = pass->GetRenderPass();
    passInfo.framebuffer = pass->GetFramebuffer();
    passInfo.renderArea.offset = {0, 0};
    passInfo.renderArea.extent = {.width = (uint32_t)images[0]->GetWidth(), .height = (uint32_t)images[0]->GetHeight()};
    if (!depthOnlyPass)
    {
        VkClearValue clearColors[2] = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        clearColors[1].depthStencil = {1.0f, 0};
        passInfo.clearValueCount = 2;
        passInfo.pClearValues = clearColors;
    }
    else
    {
        value.depthStencil = {1.0f, 0};
        passInfo.clearValueCount = 1;
        passInfo.pClearValues = &value;
    }

    vkCmdBeginRenderPass(m_CmdBuffer, &passInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::BindPipeline(IRenderPass *pPass)
{
    auto pass = (VulkanRenderPass*)pPass;

    vkCmdBindPipeline(m_CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pass->GetPipeline());
}

void CommandBuffer::BindVertexBuffer(VkBuffer buffer, VkDeviceSize offset)
{
    vkCmdBindVertexBuffers(m_CmdBuffer, 0, 1, &buffer, &offset);
}

void CommandBuffer::BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset)
{
    vkCmdBindIndexBuffer(m_CmdBuffer, buffer, offset, VK_INDEX_TYPE_UINT16);
}

void CommandBuffer::BindDescriptors(VulkanRenderPass* pPass, bool isDynamic, uint32_t offset)
{
    vkCmdBindDescriptorSets(m_CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPass->GetPipelineLayout(), 0, 1, pPass->GetCurrentDescSet(), isDynamic ? 1 : 0, isDynamic ? &offset : nullptr);
}

void CommandBuffer::PushConstants(VulkanRenderPass* pPass, size_t size, void* data, VkShaderStageFlags stage)
{
    vkCmdPushConstants(m_CmdBuffer, pPass->GetPipelineLayout(), stage, 0, size, data);
}

void CommandBuffer::Draw(int vertNum, int instanceNum, int firstVertex, int firstInstance)
{
    vkCmdDraw(m_CmdBuffer, vertNum, instanceNum, firstVertex, firstInstance);
}

void CommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
    vkCmdDrawIndexed(m_CmdBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(m_CmdBuffer);
}

void CommandBuffer::Copy(int srcOffset, int dstOffset, int size, VkBuffer src, VkBuffer dst)
{
    VkBufferCopy copy = {};
    copy.srcOffset = srcOffset;
    copy.dstOffset = dstOffset;
    copy.size = size;

    vkCmdCopyBuffer(m_CmdBuffer, src, dst, 1, &copy);

    VkMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT;
	vkCmdPipelineBarrier( 
		m_CmdBuffer, 
		VK_PIPELINE_STAGE_TRANSFER_BIT, 
		VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 
		0, 1, &barrier, 0, NULL, 0, NULL );
}

void CommandBuffer::TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	    barrier.dstAccessMask = 0;

        sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    } else {
        Logger::Log(LogLevel::FATAL, "unsupported layout transition!");
    }

    vkCmdPipelineBarrier(m_CmdBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void CommandBuffer::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkBufferImageCopy copy = {};
    copy.bufferOffset = 0;
    copy.bufferRowLength = 0;
    copy.bufferImageHeight = 0;

    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.mipLevel = 0;
    copy.imageSubresource.baseArrayLayer = 0;
    copy.imageSubresource.layerCount = 1;

    copy.imageOffset = {0, 0, 0};
    copy.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(m_CmdBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
}

void CommandBuffer::End()
{
    vkEndCommandBuffer(m_CmdBuffer);
}
