#include <Framework/Window/Window.h>

#include <Framework/Engine/Engine.h>

#include <Util/Logger.h>

#include <SDL2/SDL_vulkan.h>
#include "Window.h"

Window::Window(int iWidth, int iHeight, const char *pszName, bool bFullscreen)
{
    SDL_Init(SDL_INIT_EVERYTHING);

#ifdef RENDERER_VULKAN
    SDL_Vulkan_LoadLibrary(NULL);
#endif

    Uint32 WindowFlags = bFullscreen ? SDL_WINDOW_FULLSCREEN : 0;

#ifdef RENDERER_VULKAN
    WindowFlags |= SDL_WINDOW_VULKAN;
    Logger::Log(LogLevel::INFO, "Vulkan support enabled");
#endif

    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_pszInitialTitle = pszName;

    m_pWindow = SDL_CreateWindow(pszName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, iWidth, iHeight, WindowFlags);

    if (m_pWindow == nullptr)
    {
        Logger::Log(LogLevel::FATAL, "Failed to create platform window");
    }
}

void Window::UpdateTitle(const char *pszName)
{
    SDL_SetWindowTitle(m_pWindow, pszName);
}

const char *Window::GetInitialTitle()
{
    return m_pszInitialTitle;
}

void Window::Update()
{
}

void Window::PumpEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        // First check for exit requests
        if (event.type == SDL_QUIT)
        {
            g_pEngine->RequestShutdown();
            continue;
        }

        // Send these to the event handler in an API-agnostic way
        switch (event.type)
        {
        }
    }
}

void Window::InitVulkan()
{
}

void Window::GetInstanceExtensions(uint32_t *pCount, const char **ppExtensions)
{
    if (SDL_Vulkan_GetInstanceExtensions(m_pWindow, pCount, ppExtensions) != SDL_TRUE)
        Logger::Log(LogLevel::FATAL, "Failed to get extensions: %s", SDL_GetError());
}

void *Window::GetSurface(void* pInstance)
{
    void* ret;
    if (SDL_Vulkan_CreateSurface(m_pWindow, (VkInstance)pInstance, (VkSurfaceKHR*)&ret) != SDL_TRUE)
        Logger::Log(LogLevel::FATAL, "Failed to create vulkan window surface: %s", SDL_GetError());
    return ret;
}
