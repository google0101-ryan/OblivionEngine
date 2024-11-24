#include "../Backend.h"
#include "vulkan/VkInstance.h"
#include "vulkan/VkDevice.h"
#include "vulkan/VkSwapchain.h"
#include "vulkan/VkCommandBuffer.h"
#include "vulkan/VkProgram.h"

#include <layer0/ParallelJobTasks.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <shaderc/shaderc.hpp>

class CVkBackend : public CLayer3AppSystem<IBackend>
{
public:
    // DO NOT CALL DIRECTLY
    // Instead use SubmitCommand(BC_INIT, nullptr) or SubmitCommand(BC_SHUTDOWN, nullptr)
    virtual bool Init();
    virtual void Shutdown();

    virtual void SubmitCommand(BackendCommands_t cmd, void* pData);
private:
    CVulkanInstance m_instance;
    VkSurfaceKHR m_surface;
    CVulkanDevice m_device;
    CVulkanSwapchain m_swapChain;
    CVulkanCmdBuffer m_cmdBuffer[FRAMES_IN_FLIGHT];
    CVulkanProgram m_basePass;
};

CVkBackend g_backend;

IBackend* CreateBackend()
{
    return &g_backend;
}

bool CVkBackend::Init()
{
    m_instance.Init(GetWindow());

    m_surface = (VkSurfaceKHR)GetWindow()->GetSurface((VkInstance)m_instance);

    m_device.Init(m_instance, m_surface);

    m_swapChain.Init(m_instance, m_device, GetWindow(), m_surface);

    for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
    {
        m_cmdBuffer[i].Init(m_device);
    }

    // Compile all shaders found in the renderprogs/ folder and output them to renderbin/
    printf("Compiling shaders...\n");
    std::vector<std::string> shaders;
    GetFilesystem()->ListDirectory("renderprogs", shaders);

    CParallelListJob<std::string> job(shaders, [this](std::string& filePath)
    {
        printf("%s\n", filePath.c_str());
    });

    // Load default shaders
    // m_basePass.Init(m_instance, m_device, "renderbin/example", GetFilesystem());

    return true;
}

void CVkBackend::Shutdown()
{
    m_swapChain.Destroy(m_device);
    m_device.Destroy();
    m_instance.Destroy();
}

void CVkBackend::SubmitCommand(BackendCommands_t cmd, void *pData)
{
    switch (cmd)
    {
    case BC_INIT:
        Init();
        break;
    case BC_SHUTDOWN:
        Shutdown();
        break;
    default:
        printf("Failed to run render backend command %d\n", (int)cmd);
        exit(1);
    }
}
