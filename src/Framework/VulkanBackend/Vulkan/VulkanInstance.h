#pragma once

#include <vulkan/vulkan.h>

class VulkanInstance
{
public:
    VulkanInstance(VkInstance Handle);
    void Destroy();

    VkInstance GetHandle();
private:
    VkInstance m_Handle;
};

class VulkanInstanceBuilder
{
public:
    VulkanInstanceBuilder(const char* pAppName, uint32_t apiVersion);

    VulkanInstanceBuilder& SetExtensions(uint32_t iExtensionCount, const char** ppExtensions);
    VulkanInstanceBuilder& SetLayers(uint32_t iLayerCount, const char** ppLayers);

    VkInstance Build();
private:
    VkInstance m_Handle;
    VkInstanceCreateInfo m_CreateInfo;
    VkApplicationInfo m_AppInfo;
};