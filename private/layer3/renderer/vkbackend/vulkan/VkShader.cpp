#include "VkShader.h"
#include <cassert>

void CVulkanShader::Init(CVulkanInstance& instance, CVulkanDevice& device, std::string fileName, IFileSystem* pFileSystem)
{
    static PFN_vkCreateShadersEXT createShaders;

    if (!createShaders)
    {
        createShaders = (PFN_vkCreateShadersEXT)vkGetInstanceProcAddr(instance, "vkCreateShadersEXT");
        assert(createShaders != nullptr);
    }

    auto file = pFileSystem->OpenFileRead(fileName.c_str());

    file->seek(0, SEEK_END);
    size_t size = file->tell();
    file->seek(0, SEEK_SET);

    char* buf = new char[size];
    file->read(buf, size);
    pFileSystem->CloseFile(file);

    bool isFrag = fs::path(fileName).extension() == ".frag.spv";

    VkShaderCreateInfoEXT shaderInfo = {};
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
    shaderInfo.stage = isFrag ? VK_SHADER_STAGE_FRAGMENT_BIT : VK_SHADER_STAGE_VERTEX_BIT;
    shaderInfo.nextStage = isFrag ? 0 : VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
    shaderInfo.pCode = (void*)buf;
    shaderInfo.pName = "main";
    // TODO: Parse some kind of file that describes push constants

    CHECK_VK(createShaders(device.GetLogicalDevice(), 1, &shaderInfo, nullptr, &m_shaderObject));

    delete buf;
}

void CVulkanShader::Destroy(CVulkanInstance& instance, CVulkanDevice& device)
{
    static PFN_vkDestroyShaderEXT destroyShader;

    if (!destroyShader)
    {
        destroyShader = (PFN_vkDestroyShaderEXT)vkGetInstanceProcAddr(instance, "vkDestroyShaderEXT");
        assert(destroyShader != nullptr);
    }

    destroyShader(device.GetLogicalDevice(), m_shaderObject, nullptr);
}
