#include "VkInstance.h"
#include "../Vulkan.h"

#include <layer2/Window.h>
#include <vector>

const char* ppLayerNames[] =
{
#if DEBUG
    "VK_LAYER_KHRONOS_validation"
#endif
};
constexpr size_t layerCount = sizeof(ppLayerNames) / sizeof(ppLayerNames[0]);

void CVulkanInstance::Init(IWindow* pWindow)
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_2;
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 0);
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 0);
    appInfo.pApplicationName = "Application";
    appInfo.pEngineName = "Oblivion Engine";

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.ppEnabledLayerNames = ppLayerNames;
    createInfo.enabledLayerCount = layerCount;

    uint32_t count;
    auto ext = pWindow->QueryExtensions(&count);
    createInfo.ppEnabledExtensionNames = ext;
    createInfo.enabledExtensionCount = count;

    CHECK_VK(vkCreateInstance(&createInfo, nullptr, &m_instance));
}

void CVulkanInstance::Destroy()
{
    vkDestroyInstance(m_instance, nullptr);
}
