#pragma once

#include <layer0/layer0.h>
#include <appsystem/AppSystem.h>

#define MAX_ARGS 5

typedef std::array<CString, MAX_ARGS> CCmdArgs;

typedef void (*CommandFunc_t)(CCmdArgs& args);

#define CMD_SYSTEM_INTERFACE_NAME "CmdSysV001"
abstract_class ICommandSystem : public CLayer1AppSystem<IAppSystem>
{
public:
    virtual void AddCommand( CString name, CommandFunc_t pFunc ) = 0;
};