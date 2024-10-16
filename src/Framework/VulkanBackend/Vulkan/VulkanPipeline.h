#pragma once

#include <Framework/VulkanBackend/Vulkan/Image.h>
#include <Framework/RenderBackend/RenderPassManager.h>

#include <vulkan/vulkan.h>

#include <vector>
#include <vulkan/vulkan_core.h>

class CommandBuffer;

class VulkanShader
{
public:
    ~VulkanShader();

    void Init(std::vector<char>& fragSource, std::vector<char>& vertSource);
    void FillOutShaderStageInfo(VkPipelineShaderStageCreateInfo& vertInfo, VkPipelineShaderStageCreateInfo& fragInfo);
private:
    VkShaderModule fragShader, vertShader;
};

class VulkanRenderPass;

class VulkanPipeline
{
public:
    ~VulkanPipeline();
    void AddShader(std::string shaderName);
    void Create(VulkanRenderPass* pParent, bool hasDepth, bool enableDepthTest);

    void CommitCurrent();
    void EndFrame();
    void SetSamplerImage(int index, VulkanImage* pImage);

    VkPipeline GetPipeline() {return m_Pipeline;}
    VkPipelineLayout GetPipelineLayout() {return m_PipelineLayout;}
    VkDescriptorSet* GetDescriptorSet();
private:
    struct Binding
    {
        enum
        {
            BT_UNIFORM_BUFFER,
            BT_SAMPLER
        } type;
        int binding;
        VulkanImage* m_Image;
    };

    VulkanShader m_Shader;
    std::vector<Binding> m_Bindings;
    std::string passName;
    VkDescriptorSetLayoutBinding uboLayout;
    VkDescriptorSetLayoutBinding samplerLayout;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipeline m_Pipeline;
    VkPipelineLayout m_PipelineLayout;
    VkDescriptorPool m_DescriptorPool[2];
    int m_currentDescSet;
    VkDescriptorSet m_descriptorSets[2][16384];
};