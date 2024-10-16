#include <Framework/VulkanBackend/Vulkan/VulkanInstance.h>

#include <Util/Logger.h>
#include "VulkanInstance.h"

VulkanInstanceBuilder::VulkanInstanceBuilder(const char *pAppName, uint32_t apiVersion)
{
    m_AppInfo = {};
    m_AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    m_AppInfo.pEngineName = "OBLIVION";
    m_AppInfo.pApplicationName = pAppName;
    m_AppInfo.apiVersion = apiVersion;
    m_AppInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    m_AppInfo.applicationVersion = m_AppInfo.engineVersion;
    m_AppInfo.pNext = NULL;

    m_CreateInfo = {};
    m_CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    m_CreateInfo.pApplicationInfo = &m_AppInfo;
    m_CreateInfo.pNext = NULL;
    m_CreateInfo.flags = 0;
}

VulkanInstanceBuilder &VulkanInstanceBuilder::SetExtensions(uint32_t iExtensionCount, const char **ppExtensions)
{
    m_CreateInfo.enabledExtensionCount = iExtensionCount;
    m_CreateInfo.ppEnabledExtensionNames = ppExtensions;

    Logger::Log(LogLevel::DEBUG, "Enabled extensions:");
    for (int i = 0; i < iExtensionCount; i++)
    {
        Logger::Log(LogLevel::DEBUG, "\t%s", ppExtensions[i]);
    }

    return *this;
}

VulkanInstanceBuilder &VulkanInstanceBuilder::SetLayers(uint32_t iLayerCount, const char **ppLayers)
{
    m_CreateInfo.enabledLayerCount = iLayerCount;
    m_CreateInfo.ppEnabledLayerNames = ppLayers;
    return *this;
}

VkInstance VulkanInstanceBuilder::Build()
{
    VkResult Status = vkCreateInstance(&m_CreateInfo, nullptr, &m_Handle);
    if (Status != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to create vulkan instance (%d)", Status);
    
    return m_Handle;
}

VulkanInstance::VulkanInstance(VkInstance Handle)
{
    m_Handle = Handle;
}

void VulkanInstance::Destroy()
{
    vkDestroyInstance(m_Handle, nullptr);
}

VkInstance VulkanInstance::GetHandle()
{
    return m_Handle;
}
