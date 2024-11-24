#include "VkProgram.h"

void CVulkanProgram::Init(CVulkanInstance& instance, CVulkanDevice &device, std::string fileName, IFileSystem *pFileSystem)
{
    m_vertShader.Init(instance, device, fileName + ".vert.spv", pFileSystem);
    m_fragShader.Init(instance, device, fileName + ".frag.spv", pFileSystem);
}

void CVulkanProgram::Destroy(CVulkanInstance& instance, CVulkanDevice &device)
{
    m_vertShader.Destroy(instance, device);
    m_fragShader.Destroy(instance, device);
}
