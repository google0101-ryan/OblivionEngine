#include "Framework/Filesystem/File.h"
#include <Framework/VulkanBackend/Vulkan/CommandBuffer.h>
#include <Framework/VulkanBackend/Vulkan/VulkanPipeline.h>
#include <Framework/VulkanBackend/Vulkan/VkGodStruct.h>
#include <Framework/VulkanBackend/Vulkan/VulkanRenderPass.h>
#include <Framework/RenderBackend/Geometry.h>
#include <Framework/Filesystem/FileSystem.h>
#include <Cstdlib/Lexer.h>
#include <Util/Logger.h>

#include <cassert>
#include <cstddef>
#include <cstddef>
#include <glm/fwd.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

VulkanPipeline::~VulkanPipeline()
{
    vkDestroyPipelineLayout(g_VkState.m_Device->GetDeviceHandle(), m_PipelineLayout, nullptr);
    vkDestroyPipeline(g_VkState.m_Device->GetDeviceHandle(), m_Pipeline, nullptr);
}

void VulkanPipeline::AddShader(std::string shaderName)
{
    passName = shaderName;

    {
        auto vertFile = g_pFilesystem->OpenFileRead(("shaderbin/" + shaderName + ".vert.spv").c_str());
        if (!vertFile)
        {
            Logger::Log(LogLevel::FATAL, "Failed to find vertex shader for render pass %s!", shaderName.c_str());
        }

        vertFile->Seek(FILE_SEEK_END, 0);
        int size = vertFile->Tell();
        vertFile->Seek(FILE_SEEK_SET, 0);
        std::vector<char> vertSource(size+1);
        vertFile->Read(size, vertSource.data());
        g_pFilesystem->CloseFile(vertFile);
        
        auto fragFile = g_pFilesystem->OpenFileRead(("shaderbin/" + shaderName + ".frag.spv").c_str());
        if (!fragFile)
        {
            Logger::Log(LogLevel::FATAL, "Failed to find fragment shader for render pass %s!", shaderName.c_str());
        }

        fragFile->Seek(FILE_SEEK_END, 0);
        size = fragFile->Tell();
        fragFile->Seek(FILE_SEEK_SET, 0);
        std::vector<char> fragSource(size+1);
        fragFile->Read(size, fragSource.data());
        g_pFilesystem->CloseFile(fragFile);

        m_Shader.Init(fragSource, vertSource);
    }
}

void VulkanPipeline::Create(VulkanRenderPass* pParent, bool hasDepth, bool enableDepthTest)
{
    VkPipelineShaderStageCreateInfo vertInfo, fragInfo;
    m_Shader.FillOutShaderStageInfo(vertInfo, fragInfo);

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertInfo, fragInfo};

    VkPipelineDynamicStateCreateInfo dynamicInfo = {};
    dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicInfo.dynamicStateCount = 0;
    dynamicInfo.pDynamicStates = nullptr;
    VkVertexInputBindingDescription bindingDesc = {};
    bindingDesc.stride = sizeof(DrawVert);
    bindingDesc.binding = 0;
    bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attrs[3] = {{}, {}, {}};
    attrs[0].binding = 0;
    attrs[0].location = 0;
    attrs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrs[0].offset = offsetof(DrawVert, pos);
    attrs[1].binding = 0;
    attrs[1].location = 1;
    attrs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrs[1].offset = offsetof(DrawVert, color);
    attrs[2].binding = 0;
    attrs[2].location = 2;
    attrs[2].format = VK_FORMAT_R32G32_SFLOAT;
    attrs[2].offset = offsetof(DrawVert, texCoords);
    attrs[2].binding = 0;
    attrs[2].location = 2;
    attrs[2].format = VK_FORMAT_R32G32_SFLOAT;
    attrs[2].offset = offsetof(DrawVert, texCoords);

    VkPipelineVertexInputStateCreateInfo vtxInputStateInfo = {};
    vtxInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vtxInputStateInfo.vertexBindingDescriptionCount = 1;
    vtxInputStateInfo.pVertexBindingDescriptions = &bindingDesc;
    vtxInputStateInfo.vertexAttributeDescriptionCount = 3;
    vtxInputStateInfo.pVertexAttributeDescriptions = attrs;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)g_VkState.m_iSwapChainWidth;
    viewport.height = (float)g_VkState.m_iSwapChainHeight;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = VkExtent2D{.width = (uint32_t)g_VkState.m_iSwapChainWidth, .height = (uint32_t)g_VkState.m_iSwapChainHeight};

    VkPipelineViewportStateCreateInfo viewportInfo = {};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &scissor;
    
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    uboLayout = {};
    uboLayout.binding = 0;
    uboLayout.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayout.descriptorCount = 1;
    uboLayout.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    samplerLayout = {};
    samplerLayout.binding = 1;
    samplerLayout.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayout.descriptorCount = 1;
    samplerLayout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding bindings[] = {uboLayout, samplerLayout};
    
    VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
    descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayoutInfo.bindingCount = 2;
    descriptorLayoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(g_VkState.m_Device->GetDeviceHandle(), &descriptorLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to create descriptor set layout");

    std::vector<char> layoutBuffer;
    {
        // Read in shader layout file
        auto layout = g_pFilesystem->OpenFileRead(("shaderprogs/" + passName + ".vert.layout").c_str());
        layout->Seek(FILE_SEEK_END, 0);
        auto size = layout->Tell();
        layout->Seek(FILE_SEEK_SET, 0);
        layoutBuffer.resize(size);
        layout->Read(size, layoutBuffer.data());
        g_pFilesystem->CloseFile(layout);
    }
    layoutBuffer.push_back('\0');

    Token tk;
    Lexer lex(layoutBuffer.data());

    int bindingIndex = 0;
    if (lex.ExpectName("bindings"))
    {
        if (!lex.ExpectName("["))
        {
            Logger::Log(LogLevel::WARNING, "Invalid layout file");
            return;
        }

        while (lex.GetNextToken(&tk))
        {
            if (tk.m_pszValue == "]")
                break;
            
            Binding binding;
            binding.binding = bindingIndex++;
            
            if (tk.m_pszValue == "ubo")
            {
                printf("Found UBO!\n");
                binding.type = Binding::BT_UNIFORM_BUFFER;
            }
            else if (tk.m_pszValue == "sampler")
            {
                printf("Found sampler\n");
                binding.type = Binding::BT_SAMPLER;
            }
            else if (tk.m_pszValue == "sampler")
            {
                printf("Found sampler\n");
                binding.type = Binding::BT_SAMPLER;
            }
            else
                Logger::Log(LogLevel::FATAL, "Invalid type '%s'", tk.m_pszValue.c_str());
        
            m_Bindings.push_back(binding);
        }
    }

    std::vector<VkPushConstantRange> ranges;
    int offset = 0;
    if (lex.ExpectName("push_constants"))
    {
        if (!lex.ExpectName("["))
        {
            Logger::Log(LogLevel::WARNING, "Invalid layout file");
            return;
        }

        while (lex.GetNextToken(&tk))
        {
            if (tk.m_pszValue == "]")
                break;
            
            if (tk.m_pszValue != "{")
                Logger::Log(LogLevel::FATAL, "Invalid layout file: expected '{', got '%s' instead", tk.m_pszValue.c_str());
            
            VkPushConstantRange range = {};
            range.offset = offset;
            while (lex.GetNextToken(&tk))
            {
                if (tk.m_pszValue == "}")
                    break;
                    
                if (tk.m_pszValue == "mat4")
                {
                    range.size += sizeof(glm::mat4);
                }
                else
                    Logger::Log(LogLevel::FATAL, "Unknown push constant type '%s'", tk.m_pszValue.c_str());
            }
            offset += range.size;
            range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            ranges.push_back(range);
        }
    }

    VkPipelineLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &descriptorSetLayout;
    layoutInfo.pushConstantRangeCount = ranges.size();
    layoutInfo.pPushConstantRanges = ranges.data();

    if (vkCreatePipelineLayout(g_VkState.m_Device->GetDeviceHandle(), &layoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to create pipeline layout");

    VkPipelineDepthStencilStateCreateInfo depthState = {};
    depthState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    if (enableDepthTest)
    {
        depthState.depthTestEnable = VK_TRUE;
        depthState.depthWriteEnable = VK_TRUE;
        depthState.depthCompareOp = VK_COMPARE_OP_LESS;
        depthState.depthBoundsTestEnable = VK_FALSE;
        depthState.stencilTestEnable = VK_FALSE;
    }
    else
    {
        depthState.depthWriteEnable = VK_TRUE;
        depthState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vtxInputStateInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    if (hasDepth)
        pipelineInfo.pDepthStencilState = &depthState;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr;

    pipelineInfo.layout = m_PipelineLayout;
    pipelineInfo.renderPass = pParent->GetRenderPass();
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex = 0;

    if (vkCreateGraphicsPipelines(g_VkState.m_Device->GetDeviceHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to create graphics pipeline");

    VkDescriptorPoolSize sizes[2] = {{}, {}};
    sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    sizes[0].descriptorCount = 8192;
    sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sizes[1].descriptorCount = 12384;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.maxSets = 16384;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = sizes;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    if (vkCreateDescriptorPool(g_VkState.m_Device->GetDeviceHandle(), &poolInfo, nullptr, &m_DescriptorPool[0]) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to create descriptor pool");

    if (vkCreateDescriptorPool(g_VkState.m_Device->GetDeviceHandle(), &poolInfo, nullptr, &m_DescriptorPool[1]) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to create descriptor pool");

    m_currentDescSet = 0;
}

VkDescriptorSet* VulkanPipeline::GetDescriptorSet()
{
    return &m_descriptorSets[g_VkState.m_iFrameIndex][m_currentDescSet-1];
}

void VulkanPipeline::SetSamplerImage(int index, VulkanImage* pImage)
{
    int curIndex = 0;
    for (auto& binding : m_Bindings)
    {
        if (curIndex == index && binding.type == Binding::BT_SAMPLER)
        {
            binding.m_Image = pImage;
            break;
        }
        else if (binding.type == Binding::BT_SAMPLER)
            curIndex++;
    }
}

void VulkanPipeline::CommitCurrent()
{
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_DescriptorPool[g_VkState.m_iFrameIndex];
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;

    if (vkAllocateDescriptorSets(g_VkState.m_Device->GetDeviceHandle(), &allocInfo, &m_descriptorSets[g_VkState.m_iFrameIndex][m_currentDescSet]) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to allocate new descriptor set for object");

    VkDescriptorSet descSet = m_descriptorSets[g_VkState.m_iFrameIndex][m_currentDescSet];
    m_currentDescSet++;

    VkWriteDescriptorSet writes[32];
    VkDescriptorImageInfo images[32];
    VkDescriptorBufferInfo bufferInfos[32];

    int writeCount = 0;
    int curBufInfo = 0;
    int curImageInfo = 0;
    for (auto& binding : m_Bindings)
    {
        switch (binding.type)
        {
        case Binding::BT_UNIFORM_BUFFER:
        {
            auto& bufInfo = bufferInfos[curBufInfo++];
            bufInfo = {};
            bufInfo.buffer = g_VkState.m_UniformBuffers[g_VkState.m_iFrameIndex].GetBuffer();
            bufInfo.offset = 0;
            bufInfo.range = VK_WHOLE_SIZE;
            
            VkWriteDescriptorSet& write = writes[writeCount++];
            write = {};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.descriptorCount = 1;
            write.pBufferInfo = &bufInfo;
            write.dstBinding = binding.binding;
            write.dstSet = descSet;
            
            break;
        }
        case Binding::BT_SAMPLER:
        {
            auto& imageInfo = images[curImageInfo++];
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = binding.m_Image->GetView();
            imageInfo.sampler = binding.m_Image->GetSampler();

            VkWriteDescriptorSet& write = writes[writeCount++];
            write = {};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write.descriptorCount = 1;
            write.dstBinding = binding.binding;
            write.dstSet = descSet;
            write.pImageInfo = &imageInfo;
            
            break;
        }
        default:
            Logger::Log(LogLevel::FATAL, "Unhandled binding type %d", (int)binding.type);
        }
    }

    vkUpdateDescriptorSets(g_VkState.m_Device->GetDeviceHandle(), writeCount, writes, 0, nullptr);
}

void VulkanPipeline::EndFrame()
{
    m_currentDescSet = 0;
    if (vkResetDescriptorPool(g_VkState.m_Device->GetDeviceHandle(), m_DescriptorPool[g_VkState.m_iFrameIndex], 0) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to reset descriptor pool");
}

static VkShaderModule CreateShaderModule(std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size()-1;
    createInfo.pCode = reinterpret_cast<uint32_t*>(code.data());

    VkShaderModule ret;
    if (vkCreateShaderModule(g_VkState.m_Device->GetDeviceHandle(), &createInfo, NULL, &ret) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to create shader module!");
    
    return ret;
}

VulkanShader::~VulkanShader()
{
    vkDestroyShaderModule(g_VkState.m_Device->GetDeviceHandle(), vertShader, NULL);
    vkDestroyShaderModule(g_VkState.m_Device->GetDeviceHandle(), fragShader, NULL);
}

void VulkanShader::Init(std::vector<char> &fragSource, std::vector<char> &vertSource)
{
    fragShader = CreateShaderModule(fragSource);
    vertShader = CreateShaderModule(vertSource);
}

void VulkanShader::FillOutShaderStageInfo(VkPipelineShaderStageCreateInfo &vertInfo, VkPipelineShaderStageCreateInfo &fragInfo)
{
    vertInfo = {};
    vertInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertInfo.module = vertShader;
    vertInfo.pName = "main";
    
    fragInfo = {};
    fragInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragInfo.module = fragShader;
    fragInfo.pName = "main";
}
