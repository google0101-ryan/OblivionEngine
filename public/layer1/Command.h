#pragma once

#include <appsystem/AppSystem.h>
#include <array>
#include <string>

#define MAX_ARGS 20
typedef std::array<std::string, MAX_ARGS> CmdArgs_t;

typedef void (*Command_t)(CmdArgs_t& args);

#define COMMAND_INTERFACE_NAME "CmdSysV001"
abstract_class ICommandSystem : public IAppSystem
{
public:
    virtual void AddCommand(const char* pCmdName, Command_t pCommand) = 0;
    virtual void ExecuteCommand(const char* pCmdText) = 0;
};