#include <Framework/RenderBackend/RenderPassManager.h>
#include <Framework/VulkanBackend/Vulkan/VulkanRenderPass.h>

IRenderPass* RenderPassManager::CreateRenderPass()
{
    return new VulkanRenderPass;
}