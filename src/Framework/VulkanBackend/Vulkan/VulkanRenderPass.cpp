#include "Framework/VulkanBackend/Vulkan/Image.h"
#include <Framework/VulkanBackend/Vulkan/VulkanRenderPass.h>
#include <Framework/VulkanBackend/Vulkan/VulkanFormat.h>
#include <Framework/VulkanBackend/Vulkan/VkGodStruct.h>
#include <Framework/RenderBackend/RenderGraphImage.h>

#include <Util/Logger.h>
#include <vulkan/vulkan_core.h>

VulkanRenderPass::~VulkanRenderPass()
{
    vkDestroyRenderPass(g_VkState.m_Device->GetDeviceHandle(), m_RenderPass, nullptr);
    vkDestroyFramebuffer(g_VkState.m_Device->GetDeviceHandle(), m_FrameBuffer, nullptr);
}

void VulkanRenderPass::AddShader(std::string shaderName)
{
    m_Pipeline.AddShader(shaderName);
}

void VulkanRenderPass::Create(std::vector<IRenderImage*> pOutputImages, bool isOutputPass, std::vector<IResource*> pInputs, std::vector<IResource*> pOutputs, bool enableDepthTest)
{
    m_pOutputImages = pOutputImages;
    // TODO: Sanity check pOutputImages and ensure they all share the same format, width, height, etc

    std::vector<VkAttachmentDescription> outputImages;
    for (int i = 0; i < pOutputImages.size(); i++)
    {
        VkAttachmentDescription outputImage = {};
        outputImage.format = FormatToVulkan(pOutputImages[i]->GetFormat());
        outputImage.samples = VK_SAMPLE_COUNT_1_BIT;
        outputImage.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        outputImage.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        outputImage.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        if (isOutputPass)
            outputImage.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        else
            outputImage.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // We will be sampled from by some other shader
        outputImages.push_back(outputImage);
    }
    int depthIndex = -1, actualDepthIndex = -1;
    for (int i = 0; i < pOutputs.size(); i++)
    {
        VkAttachmentDescription desc = {};
        desc.format = FormatToVulkan(((IRenderImage*)pOutputs[i])->GetFormat());
        desc.samples = VK_SAMPLE_COUNT_1_BIT;
        desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        if (pOutputs[i]->GetName() == "_depth")
        {
            desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthIndex = i;
            actualDepthIndex = outputImages.size();
        }
        else
            desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        outputImages.push_back(desc);
        m_pOutputImages.push_back((IRenderImage*)pOutputs[i]);
    }
    
    VkAttachmentReference outputRef = {};
    outputRef.attachment = 0; // TODO: Proper binding assignment
    outputRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthRef;
    depthRef.attachment = actualDepthIndex;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    if (outputImages.size() == 1 && outputImages[0].finalLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        subpass.pColorAttachments = nullptr;
        subpass.colorAttachmentCount = 0;
    }
    else
    {
        subpass.pColorAttachments = &outputRef;
        subpass.colorAttachmentCount = 1;
    }
    if (depthIndex != -1)
    {
        subpass.pDepthStencilAttachment = &depthRef;
    }

    VkSubpassDependency dependency = {};
    dependency.dstSubpass = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;

    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = outputImages.size();
    createInfo.pAttachments = outputImages.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(g_VkState.m_Device->GetDeviceHandle(), &createInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to create render pass");

    m_Pipeline.Create(this, depthIndex != -1, enableDepthTest);

    std::vector<VkFramebufferAttachmentImageInfo> fbAttachmentImageInfo;
    for (auto& image : pOutputImages)
    {
        m_OutputFormats.push_back(FormatToVulkan(image->GetFormat()));

        VkFramebufferAttachmentImageInfo attachments_image_info = {};
        attachments_image_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENT_IMAGE_INFO;
        attachments_image_info.width = image->GetWidth();
        attachments_image_info.height = image->GetHeight();
        attachments_image_info.layerCount = 1;
        attachments_image_info.viewFormatCount = 1;
        attachments_image_info.pViewFormats = &m_OutputFormats[m_OutputFormats.size()-1];
        attachments_image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        fbAttachmentImageInfo.push_back(attachments_image_info);
    }

    auto depthFormat = VK_FORMAT_UNDEFINED;
    if (depthIndex != -1)
    {
        depthFormat = FormatToVulkan(((IRenderImage*)pOutputs[depthIndex])->GetFormat());
        auto image = (IRenderImage*)pOutputs[depthIndex];
        
        VkFramebufferAttachmentImageInfo depth_attachment_info = {};
        depth_attachment_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENT_IMAGE_INFO;
        depth_attachment_info.width = image->GetWidth();
        depth_attachment_info.height = image->GetHeight();
        depth_attachment_info.layerCount = 1;
        depth_attachment_info.viewFormatCount = 1;
        depth_attachment_info.pViewFormats = &depthFormat;
        depth_attachment_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        fbAttachmentImageInfo.push_back(depth_attachment_info);
    }

    // Create framebuffer (we use imageless framebuffers)
    VkFramebufferAttachmentsCreateInfo fbAttachmentInfo = {};
    fbAttachmentInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENTS_CREATE_INFO;
    fbAttachmentInfo.attachmentImageInfoCount = fbAttachmentImageInfo.size();
    fbAttachmentInfo.pAttachmentImageInfos = fbAttachmentImageInfo.data();

    VkFramebufferCreateInfo fbInfo = {};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.flags = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT;
    fbInfo.pNext = &fbAttachmentInfo;
    fbInfo.renderPass = m_RenderPass;
    fbInfo.attachmentCount = fbAttachmentImageInfo.size();
    fbInfo.width = m_pOutputImages[0]->GetWidth();
    fbInfo.height = m_pOutputImages[0]->GetHeight();
    fbInfo.layers = 1;
    m_iAttachmentCount = fbAttachmentImageInfo.size();

    if (vkCreateFramebuffer(g_VkState.m_Device->GetDeviceHandle(), &fbInfo, nullptr, &m_FrameBuffer) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to create render pass framebuffer");
}

void VulkanRenderPass::CommitAll()
{
    m_Pipeline.CommitCurrent();
}

void VulkanRenderPass::SetImageAtIndex(int index, IRenderImage* pImage)
{
    m_Pipeline.SetSamplerImage(index, (VulkanImage*)pImage->GetAPIData());
}