#pragma once

#include <appsystem/AppSystem.h>

class IWindow : public IAppSystem
{
public:
    virtual void PumpEvents() = 0;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual void* GetApiHandle() = 0;
#if VULKAN_RENDERER
    virtual void* GetSurface(void* pInstance) = 0;
    virtual const char* const* QueryExtensions(uint32_t* pCount) = 0;
#endif
};

#define WINDOW_INTERFACE_NAME "WinSysV001"