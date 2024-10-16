#pragma once

#include "Framework/RenderBackend/RenderGraphImage.h"
#include <vulkan/vulkan.h>
#include <Framework/RenderBackend/RenderPassManager.h>
#include <Framework/VulkanBackend/Vulkan/VulkanPipeline.h>
#include <vulkan/vulkan_core.h>

class VulkanRenderPass : public IRenderPass
{
public:
    ~VulkanRenderPass();

    void AddShader(std::string shaderName) override;
    void Create(std::vector<IRenderImage*> pOutputImages, bool isOutputPass, std::vector<IResource*> pInputs, std::vector<IResource*> pOutputs, bool enableDepthTest) override;
    void CommitAll() override;
    void SetImageAtIndex(int index, IRenderImage* pImage) override;
    void EndFrame() override { m_Pipeline.EndFrame(); }

    VkRenderPass GetRenderPass() {return m_RenderPass;}
    VkFramebuffer GetFramebuffer() {return m_FrameBuffer;}
    VkPipeline GetPipeline() {return m_Pipeline.GetPipeline();}
    std::vector<IRenderImage*>& GetOutputs() {return m_pOutputImages;}
    VkPipelineLayout GetPipelineLayout() {return m_Pipeline.GetPipelineLayout();}
    VkDescriptorSet* GetCurrentDescSet() {return m_Pipeline.GetDescriptorSet();}
    int GetAttachmentCount() {return m_iAttachmentCount;}
private:
    VulkanPipeline m_Pipeline;
    std::vector<VkFramebuffer> m_Framebuffers;
    VkRenderPass m_RenderPass;
    VkFramebuffer m_FrameBuffer;
    std::vector<VkFormat> m_OutputFormats;
    std::vector<IRenderImage*> m_pOutputImages;
    int m_iAttachmentCount;
};