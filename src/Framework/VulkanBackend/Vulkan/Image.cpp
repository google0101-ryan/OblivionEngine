#include "Framework/RenderBackend/RenderFormat.h"
#include "Framework/VulkanBackend/Vulkan/Buffer.h"
#include "Framework/VulkanBackend/Vulkan/CommandBuffer.h"
#include "Framework/VulkanBackend/Vulkan/VulkanFormat.h"
#include "Util/Logger.h"
#include <Framework/VulkanBackend/Vulkan/Image.h>
#include <Framework/RenderBackend/RenderGraphImage.h>
#include <Framework/VulkanBackend/Vulkan/VkGodStruct.h>
#include <cstring>
#include <vulkan/vulkan_core.h>

VkImageUsageFlags ImageUsageToVulkan(ImageUsage usage)
{
    switch (usage)
    {
    case IU_DEPTH_ATTACHMENT:
        return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    case IU_REGULAR_IMAGE:
        return VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    default:
        Logger::Log(LogLevel::FATAL, "Failed to translate image usage");
    }
}

extern VmaAllocator allocator;

VkSamplerAddressMode AddressModeToVulkan(ImageAddressType addr)
{
    switch (addr)
    {
    case IA_REPEAT:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case IA_BORDER:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    case IA_CLAMP:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case IA_MIRROR:
        return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    }
}

VkFilter FilterToVulkan(ImageFilterType filter)
{
    switch (filter)
    {
    case IF_LINEAR:
        return VK_FILTER_LINEAR;
    case IF_NEAREST:
        return VK_FILTER_NEAREST;
    }
}

VulkanImage::VulkanImage(int iWidth, int iHeight, ColorFormat format, ImageUsage usage, VkImageAspectFlags aspectFlags, bool shaderAttachment, ImageAddressType addressType, ImageFilterType filter)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_Format = format;

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = m_iWidth;
    imageCreateInfo.extent.height = m_iHeight;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = FormatToVulkan(m_Format);
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = ImageUsageToVulkan(usage);
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.flags = 0;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    allocInfo.priority = 1.0f;

    if (vmaCreateImage(allocator, &imageCreateInfo, &allocInfo, &m_Image, &m_Alloc, &m_AllocInfo) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to create image!");

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_Image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = FormatToVulkan(m_Format);
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(g_VkState.m_Device->GetDeviceHandle(), &viewInfo, nullptr, &m_View) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to create image view");

    if (shaderAttachment)
    {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.addressModeU = AddressModeToVulkan(addressType);
        samplerInfo.addressModeV = AddressModeToVulkan(addressType);
        samplerInfo.addressModeW = AddressModeToVulkan(addressType);
        samplerInfo.magFilter = FilterToVulkan(filter);
        samplerInfo.minFilter = FilterToVulkan(filter);
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = 16; // TODO: detect this from the GPU
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;
        
        if (vkCreateSampler(g_VkState.m_Device->GetDeviceHandle(), &samplerInfo, nullptr, &m_Sampler) != VK_SUCCESS)
            Logger::Log(LogLevel::FATAL, "Failed to create texture sampler");
    }
}

VulkanImage::VulkanImage(VkImage &image, VkImageView &view, int iWidth, int iHeight, ColorFormat format)
{
    m_Image = image;
    m_View = view;
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_Format = format;
}

void VulkanImage::CopyToImage(void* imageData, int size)
{
    VkFence tmpFence;
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    vkCreateFence(g_VkState.m_Device->GetDeviceHandle(), &fenceInfo, nullptr, &tmpFence);

    CommandBuffer tmpCmdBuffer;
    tmpCmdBuffer.Create();

    VulkanBuffer stagingBuffer;
    stagingBuffer.Create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true, true);
    auto data = stagingBuffer.GetMappedData();
    std::memcpy(data, imageData, size);

    tmpCmdBuffer.Begin();
    tmpCmdBuffer.TransitionImageLayout(m_Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    tmpCmdBuffer.CopyBufferToImage(stagingBuffer.GetBuffer(), m_Image, m_iWidth, m_iHeight);
    tmpCmdBuffer.TransitionImageLayout(m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    tmpCmdBuffer.End();
    tmpCmdBuffer.Submit(0, nullptr, 0, nullptr, nullptr, tmpFence);

    vkWaitForFences(g_VkState.m_Device->GetDeviceHandle(), 1, &tmpFence, VK_TRUE, UINT64_MAX);

    tmpCmdBuffer.Destroy();
    stagingBuffer.Destroy();
}

void* CreateAPIData(IRenderImage* pImage)
{
    return (void*)new VulkanImage();
}