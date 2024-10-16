#pragma once

#include <cstdint>

#include <SDL2/SDL.h>

class Window
{
public:
    Window(int iWidth, int iHeight, const char* pszName, bool bFullscreen);

    void UpdateTitle(const char* pszName);
    const char* GetInitialTitle();

    void Update();
    void PumpEvents();

    const int GetWidth() const {return m_iWidth;}
    const int GetHeight() const {return m_iHeight;}

    // Get API specific handle
    void* GetAPIHandle();

    // Vulkan support
    void InitVulkan();
    void GetInstanceExtensions(uint32_t* pCount, const char** ppExtensions);
    void* GetSurface(void* pInstance);
private:
    SDL_Window* m_pWindow;
    int m_iWidth, m_iHeight;
    const char* m_pszInitialTitle;
};