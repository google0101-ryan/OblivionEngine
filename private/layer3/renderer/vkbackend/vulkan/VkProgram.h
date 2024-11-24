#pragma once

#include "VkShader.h"

// Just contains two CVulkanShader objects to help manage them

class CVulkanProgram
{
public:
    void Init(CVulkanInstance& instance, CVulkanDevice& device, std::string fileName, IFileSystem* pFileSystem);
    void Destroy(CVulkanInstance& instance, CVulkanDevice& device);
private:
    CVulkanShader m_vertShader;
    CVulkanShader m_fragShader;
};