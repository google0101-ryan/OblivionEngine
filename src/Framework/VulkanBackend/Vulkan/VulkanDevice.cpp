#include "VulkanDevice.h"

#include <Framework/VulkanBackend/Vulkan/VulkanInstance.h>
#include <Framework/VulkanBackend/Vulkan/VkGodStruct.h>
#include <Framework/VulkanBackend/Vulkan/VulkanFormat.h>
#include <Util/Logger.h>

#include <vector>

const char* defaultExtensions[] =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

int defaultExtensionSize = sizeof(defaultExtensions) / sizeof(defaultExtensions[0]);

VulkanDeviceBuilder::VulkanDeviceBuilder()
{
    m_CreateInfo = {};
    m_CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
}

VulkanDeviceBuilder &VulkanDeviceBuilder::SetLayers(uint32_t iLayerCount, const char **ppLayers)
{
    m_CreateInfo.enabledLayerCount = iLayerCount;
    m_CreateInfo.ppEnabledLayerNames = ppLayers;
    return *this;
}

VkDebugUtilsMessengerEXT debugMessenger;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    Logger::Log(LogLevel::INFO, "validation layer: %s", pCallbackData->pMessage);

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VulkanDevice *VulkanDeviceBuilder::Build(VulkanInstance *pInstance)
{
    
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional

    if (auto result = CreateDebugUtilsMessengerEXT(g_VkState.m_Instance->GetHandle(), &createInfo, nullptr, &debugMessenger); result != VK_SUCCESS) {
        Logger::Log(LogLevel::FATAL, "failed to set up debug messenger %d!", result);
    }
    
    VkPhysicalDevice chosen;
    VkDevice logicalGpu;

    uint32_t iGpuCount;
    vkEnumeratePhysicalDevices(pInstance->GetHandle(), &iGpuCount, nullptr);
    if (iGpuCount <= 0)
    {
        return nullptr;
    }
    std::vector<VkPhysicalDevice> possibleDevices(iGpuCount);
    vkEnumeratePhysicalDevices(pInstance->GetHandle(), &iGpuCount, &possibleDevices[0]);

    // TODO: Actually choose a device based on user-provided criteria
    chosen = possibleDevices[0];

    VkPhysicalDeviceProperties gpuProps;
    vkGetPhysicalDeviceProperties(chosen, &gpuProps);
    Logger::Log(LogLevel::INFO, "Selected device \"%s\"", gpuProps.deviceName);

    uint32_t graphicsQueueFamily, presentQueueIdx;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(chosen, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(chosen, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (auto& family : queueFamilies)
    {
        if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsQueueFamily = i;
        }

        VkBool32 surfSupported;
        vkGetPhysicalDeviceSurfaceSupportKHR(chosen, i, g_VkState.m_Surface, &surfSupported);

        if (surfSupported)
        {
            presentQueueIdx = i;
        }

        i++;
    }

    float queuePrio = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueInfos = {};
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePrio;
    queueInfo.queueFamilyIndex = graphicsQueueFamily;
    queueInfos.push_back(queueInfo);
    if (graphicsQueueFamily != presentQueueIdx)
    {
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &queuePrio;
        queueInfo.queueFamilyIndex = presentQueueIdx;
        queueInfos.push_back(queueInfo);
    }

    VkPhysicalDeviceVulkan12Features features12 = {};
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.imagelessFramebuffer = VK_TRUE;
    features12.descriptorIndexing = VK_TRUE;
    features12.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
    features12.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
    features12.shaderUniformBufferArrayNonUniformIndexing = VK_TRUE;
    features12.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
    features12.shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
    features12.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;

    VkPhysicalDeviceFeatures2 features = {};
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &features12;
    features.features.samplerAnisotropy = VK_TRUE;
    
    m_CreateInfo.queueCreateInfoCount = queueInfos.size();
    m_CreateInfo.pQueueCreateInfos = queueInfos.data();
    m_CreateInfo.pNext = &features;
    m_CreateInfo.enabledExtensionCount = defaultExtensionSize;
    m_CreateInfo.ppEnabledExtensionNames = defaultExtensions;
    
    if (vkCreateDevice(chosen, &m_CreateInfo, nullptr, &logicalGpu) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to create logical device");

    return new VulkanDevice(logicalGpu, chosen, presentQueueIdx, graphicsQueueFamily);
}

VulkanDevice::VulkanDevice(VkDevice &device, VkPhysicalDevice &physDevice, uint32_t presentQueueIdx, uint32_t graphicsQueueIdx)
{
    m_Device = device;
    m_PhysDevice = physDevice;
    m_PresentQueueIdx = presentQueueIdx;
    m_GraphicsQueueIdx = graphicsQueueIdx;

    vkGetDeviceQueue(m_Device, m_GraphicsQueueIdx, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_Device, presentQueueIdx, 0, &m_presentQueue);
}

void VulkanDevice::Destroy()
{
    vkDestroyDevice(m_Device, nullptr);
}

VkSurfaceCapabilitiesKHR VulkanDevice::GetSurfaceCaps(VkSurfaceKHR surf)
{
    VkSurfaceCapabilitiesKHR ret;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysDevice, surf, &ret);
    return ret;
}

std::vector<DisplayFormat> VulkanDevice::GetFormats(VkSurfaceKHR surface)
{
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysDevice, surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysDevice, surface, &formatCount, &formats[0]);

    std::vector<DisplayFormat> ret;

    for (int i = 0; i < formatCount; i++)
    {
        DisplayFormat format;
        format.m_Format = VulkanToFormat(formats[i].format);
        format.m_ColorSpace = VulkanToColorSpace(formats[i].colorSpace);
        ret.push_back(format);
    }

    return ret;
}

SwapChainPresentMode VkSwapModeToOblivion(VkPresentModeKHR mode)
{
    switch (mode)
    {
    case VK_PRESENT_MODE_FIFO_KHR:
        return SwapChainPresentMode::PRESENT_MODE_FIFO;
    case VK_PRESENT_MODE_MAILBOX_KHR:
        return SwapChainPresentMode::PRESENT_MODE_MAILBOX;
    case VK_PRESENT_MODE_IMMEDIATE_KHR:
        return SwapChainPresentMode::PRESENT_MODE_IMMEDIATE;
    default:
        Logger::Log(LogLevel::FATAL, "Failed to convert present mode %d\n", mode);
        return SwapChainPresentMode::PRESENT_MODE_NONE;
    }
}

SwapChainPresentMode VulkanDevice::PickPresentMode(VkSurfaceKHR surf, SwapChainPresentMode preferred)
{
    uint32_t modeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysDevice, surf, &modeCount, NULL);
    std::vector<VkPresentModeKHR> modes(modeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysDevice, surf, &modeCount, &modes[0]);

    for (auto& m : modes)
    {
        SwapChainPresentMode mode = VkSwapModeToOblivion(m);
        if (mode == preferred)
            return mode;
    }

    printf("Using fallback swap mode\n");
    return VkSwapModeToOblivion(modes[0]);
}

void VulkanDevice::WaitIdle()
{
    vkDeviceWaitIdle(m_Device);
}