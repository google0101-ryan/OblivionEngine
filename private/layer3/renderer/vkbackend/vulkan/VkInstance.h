#pragma once

#include <vulkan/vulkan.h>

class IWindow;

class CVulkanInstance
{
public:
    void Init(IWindow* pWindow);
    void Destroy();

    operator VkInstance() const
    {
        return m_instance;
    }
private:
    VkInstance m_instance;
};