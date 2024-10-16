#pragma once

#include "AppComm.h"

class IApplication
{
public:
    virtual void Init(AppCommunication* pAppComms) = 0;
    virtual void Tick() = 0;
    virtual void Shutdown() = 0;
};

#define EXPOSE_APPCLASS(name) \
static name s_AppInstance; \
static IApplication* app = &s_AppInstance; \
extern "C" IApplication* GetMainApp() \
{ \
    return app; \
}