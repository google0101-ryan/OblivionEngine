#pragma once

#include <appsystem/AppSystem.h>

class IWindow : public IAppSystem
{
public:
    virtual void PumpEvents() = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
};

#define WINDOW_INTERFACE_NAME "WinSysV001"