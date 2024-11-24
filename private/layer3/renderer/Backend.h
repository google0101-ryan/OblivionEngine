#pragma once

#include <layer0/Thread.h>
#include <layer3/layer3.h>

typedef enum BackendCommands_t
{
    BC_INIT,
    BC_SHUTDOWN
};

// Unlike most app systems, this one stays private
abstract_class IBackend : public IAppSystem 
{
public:
    virtual void SubmitCommand(BackendCommands_t cmd, void* pData) = 0;
};

// Implemented per-platform
IBackend* CreateBackend();