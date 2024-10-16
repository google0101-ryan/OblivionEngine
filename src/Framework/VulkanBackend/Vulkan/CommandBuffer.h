#pragma once

#include <vulkan/vulkan.h>
#include <Framework/VulkanBackend/Vulkan/VulkanRenderPass.h>
#include <vulkan/vulkan_core.h>

enum WaitStages
{
    WAIT_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
};

class CommandBuffer
{
public:
    void Create();
    void Destroy();

    void Reset();
    void Submit(int waitSemaCount, VkSemaphore* pWaitSemas, int signalSemaCount, VkSemaphore* signalSemas, WaitStages* stages, VkFence waitFence);

    void Begin();
    void BeginRenderPass(IRenderPass* pPass, bool depthOnlyPass);
    void BindPipeline(IRenderPass* pPass);
    void BindVertexBuffer(VkBuffer buffer, VkDeviceSize offset);
    void BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset);
    void BindDescriptors(VulkanRenderPass* pPass, bool isDynamic, uint32_t offset);
    void PushConstants(VulkanRenderPass* pPass, size_t size, void* data, VkShaderStageFlags stage);
    void Draw(int vertNum, int instanceNum, int firstVertex, int firstInstance);
    void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
    void EndRenderPass();
    void Copy(int srcOffset, int dstOffset, int size, VkBuffer src, VkBuffer dst);
    void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void End();
private:
    VkCommandPool m_CmdPool;
    VkCommandBuffer m_CmdBuffer;
};