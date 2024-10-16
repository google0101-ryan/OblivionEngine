#pragma once

#include "Framework/VulkanBackend/Vulkan/Buffer.h"
#include <Framework/RenderBackend/RenderFormat.h>
#include <vulkan/vulkan.h>
#include <Framework/RenderBackend/RenderGraphImage.h>

enum ImageUsage
{
    IU_DEPTH_ATTACHMENT,
    IU_REGULAR_IMAGE
};

enum ImageAddressType
{
    IA_REPEAT,
    IA_MIRROR,
    IA_CLAMP,
    IA_BORDER  
};

enum ImageFilterType
{
    IF_NEAREST,
    IF_LINEAR
};

class VulkanImage : public IImageData
{
public:
    // Mainly used for swapchain
    VulkanImage(int iWidth, int iHeight, ColorFormat format, ImageUsage usage, VkImageAspectFlags aspectFlags, bool shaderAttachment, ImageAddressType addressType = IA_REPEAT, ImageFilterType filter = IF_LINEAR);
    VulkanImage(VkImage& image, VkImageView& view, int iWidth, int iHeight, ColorFormat format);
    VulkanImage() { /* TODO */ }

    VkImage GetImage() const {return m_Image;}
    VkImageView GetView() const {return m_View;}
    VkSampler GetSampler() const {return m_Sampler;}
    void CopyToImage(void* imageData, int size);

    virtual int GetWidth() const override { return m_iWidth; }
    virtual int GetHeight() const override { return m_iHeight; }
    virtual ColorFormat GetFormat() const override { return m_Format; }
private:
    VkImage m_Image;
    VkImageView m_View;
    VkSampler m_Sampler;
    int m_iWidth, m_iHeight;
    ColorFormat m_Format;
    VmaAllocation m_Alloc;
    VmaAllocationInfo m_AllocInfo;
};