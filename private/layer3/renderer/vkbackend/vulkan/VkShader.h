#pragma once

#include "../Vulkan.h"
#include "VkDevice.h"
#include "VkInstance.h"
#include <string>
#include <layer2/FileSystem.h>

enum ShaderStage
{
    SHADER_STAGE_VERTEX,
    SHADER_STAGE_FRAGMENT
};

class CVulkanShader
{
public:
    void Init(CVulkanInstance& instance, CVulkanDevice& device, std::string fileName, IFileSystem* pFileSystem);
    void Destroy(CVulkanInstance& instance, CVulkanDevice& device);
private:
    VkShaderEXT m_shaderObject;
};