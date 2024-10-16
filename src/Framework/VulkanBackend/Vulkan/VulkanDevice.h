#pragma once

#include <Framework/RenderBackend/RenderFormat.h>

#include <vulkan/vulkan.h>
#include <vector>

#define MODULE "VKDEV"
#include <Util/Logger.h>

// Forward declarations
class VulkanInstance;

enum class SwapChainPresentMode
{
    PRESENT_MODE_NONE = -1,
    PRESENT_MODE_FIFO = 1,
    PRESENT_MODE_MAILBOX,
    PRESENT_MODE_IMMEDIATE
};

inline VkPresentModeKHR PresentModeToVulkan(SwapChainPresentMode mode)
{
    switch (mode)
    {
    case SwapChainPresentMode::PRESENT_MODE_FIFO:
        return VK_PRESENT_MODE_FIFO_KHR;
    case SwapChainPresentMode::PRESENT_MODE_MAILBOX:
        return VK_PRESENT_MODE_MAILBOX_KHR;
    case SwapChainPresentMode::PRESENT_MODE_IMMEDIATE:
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    default:
        Logger::Log(LogLevel::FATAL, "Failed to convert present mode %d\n", mode);
        return VK_PRESENT_MODE_FIFO_KHR;
    }
}

struct DisplayFormat
{
    ColorFormat m_Format;
    ColorSpace m_ColorSpace;
};

// This class combines both the VkDevice and VkPhysicalDevice into one class

class VulkanDevice
{
public:
    VulkanDevice(VkDevice& device, VkPhysicalDevice& physDevice, uint32_t presentQueueIdx, uint32_t m_GraphicsQueueIdx);
    void Destroy();

    VkDevice GetDeviceHandle() const {return m_Device;}
    VkPhysicalDevice GetPhysDevice() const {return m_PhysDevice;}
    uint32_t GetPresentQueueIdx() const {return m_PresentQueueIdx;}
    uint32_t GetGraphicsQueueIdx() const {return m_GraphicsQueueIdx;}

    VkSurfaceCapabilitiesKHR GetSurfaceCaps(VkSurfaceKHR surf);
    std::vector<DisplayFormat> GetFormats(VkSurfaceKHR surface);
    SwapChainPresentMode PickPresentMode(VkSurfaceKHR surf, SwapChainPresentMode preferred);

    VkQueue GetGraphicsQueue() {return m_graphicsQueue;}
    VkQueue GetPresentQueue() {return m_presentQueue;}

    void WaitIdle();
private:
    VkDevice m_Device;
    VkPhysicalDevice m_PhysDevice;

    uint32_t m_PresentQueueIdx, m_GraphicsQueueIdx;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
};

class VulkanDeviceBuilder
{
public:
    VulkanDeviceBuilder();

    VulkanDeviceBuilder& SetLayers(uint32_t iLayerCount, const char** ppLayers);
    VulkanDevice* Build(VulkanInstance* pInstance);
private:
    VkDeviceCreateInfo m_CreateInfo;
};

#undef MODULE