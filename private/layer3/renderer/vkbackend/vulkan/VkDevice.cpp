#include "VkDevice.h"
#include "../Vulkan.h"

#include <vector>
#include <map>

static const char* deviceExtensions[] =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_SHADER_OBJECT_EXTENSION_NAME
};
static int deviceExtensionCount = sizeof(deviceExtensions) / sizeof(deviceExtensions[0]);

int ScoreDevice(VkPhysicalDevice dev)
{
    int score = 0;

    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(dev, &props);

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;
    
    return score;
}

void CVulkanDevice::Init(CVulkanInstance& instance, VkSurfaceKHR surface)
{
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Find a device to use
    std::map<int, VkPhysicalDevice> scored;
    for (auto& dev : devices)
    {
        scored[ScoreDevice(dev)] = dev;
    }

    // Use the highest scoring device
    m_device = (scored.rbegin())->second;

    if (m_device == VK_NULL_HANDLE)
    {
        printf("Failed to find a suitable vulkan capable device!\n");
        exit(1);
    }

    uint32_t queueCount;
    vkGetPhysicalDeviceQueueFamilyProperties(m_device, &queueCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueProps(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_device, &queueCount, queueProps.data());

    for (uint32_t i = 0; i < queueCount; i++)
    {
        auto& props = queueProps[i];

        if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            m_graphicsQueue = i;
        }

        VkBool32 presentSupported;
        vkGetPhysicalDeviceSurfaceSupportKHR(m_device, i, surface, &presentSupported);

        if (presentSupported == VK_TRUE)
            m_presentQueue = i;
    }

    float prio = 1.0f;
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueCount = 1;
    queueInfo.queueFamilyIndex = m_graphicsQueue;
    queueInfo.pQueuePriorities = &prio;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueInfo;
    createInfo.ppEnabledExtensionNames = deviceExtensions;
    createInfo.enabledExtensionCount = deviceExtensionCount;

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.geometryShader = VK_TRUE;
    deviceFeatures.tessellationShader = VK_TRUE;
    deviceFeatures.sampleRateShading = true;
    deviceFeatures.multiDrawIndirect = true;
    deviceFeatures.drawIndirectFirstInstance = true;
    deviceFeatures.depthBiasClamp = true;
    deviceFeatures.fillModeNonSolid = true;
    deviceFeatures.samplerAnisotropy = true;
    deviceFeatures.textureCompressionBC = true;
    deviceFeatures.vertexPipelineStoresAndAtomics = true;
    deviceFeatures.shaderInt64 = true;

    VkPhysicalDeviceVulkan11Features feat11 = {};
    feat11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    feat11.pNext = &deviceFeatures;
    feat11.storageBuffer16BitAccess = true;
    feat11.samplerYcbcrConversion = true;
    feat11.shaderDrawParameters = true;

    VkPhysicalDeviceVulkan12Features feat12 = {};
    feat12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    feat12.pNext = &feat11;
    feat12.drawIndirectCount = true;
    feat12.storageBuffer8BitAccess = true;
    feat12.uniformAndStorageBuffer8BitAccess = true;
    feat12.descriptorIndexing = true;
    feat12.shaderSampledImageArrayNonUniformIndexing = true;
    feat12.descriptorBindingSampledImageUpdateAfterBind = true;
    feat12.descriptorBindingStorageImageUpdateAfterBind = true;
    feat12.descriptorBindingUpdateUnusedWhilePending = true;
    feat12.descriptorBindingPartiallyBound = true;
    feat12.descriptorBindingVariableDescriptorCount = true;
    feat12.runtimeDescriptorArray = true;
    feat12.scalarBlockLayout = true;
    feat12.uniformBufferStandardLayout = true;
    feat12.timelineSemaphore = true;
    feat12.bufferDeviceAddress = true;

    VkPhysicalDeviceVulkan13Features feat13 = {};
    feat13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    feat13.pNext = &feat12;
    feat13.subgroupSizeControl = true;
    feat13.synchronization2 = true;
    feat13.dynamicRendering = true;
    feat13.maintenance4 = true;

    createInfo.pEnabledFeatures = &deviceFeatures;
    
    CHECK_VK(vkCreateDevice(m_device, &createInfo, nullptr, &m_logicalDevice));
}

void CVulkanDevice::Destroy()
{
    vkDestroyDevice(m_logicalDevice, nullptr);
}

VkSurfaceFormatKHR CVulkanDevice::GetSwapFormat(VkSurfaceKHR surf)
{
    VkFormat desired = VK_FORMAT_R8G8B8_SRGB;

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_device, surf, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_device, surf, &formatCount, formats.data());

    for (auto& fmt : formats)
    {
        if (fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && fmt.format == desired)
            return fmt;
    }

    return formats[0];
}

VkPresentModeKHR CVulkanDevice::GetSwapMode(VkSurfaceKHR surf)
{
    VkPresentModeKHR desired = VK_PRESENT_MODE_MAILBOX_KHR;

    uint32_t presentCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_device, surf, &presentCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_device, surf, &presentCount, presentModes.data());

    for (auto& mode : presentModes)
    {
        if (mode == desired)
            return mode;
    }

    return presentModes[0];
}
