#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct CmdArgs
{
    int argc;
    std::vector<std::string> argv;
};

class ICommand
{
public:
    virtual void Execute(CmdArgs& args) = 0;
};

class CommandManager
{
public:
    void AddCommand(std::string cmdName, ICommand* cmd);

    void ExecuteCommand(std::string commandText);
private:
    std::unordered_map<std::string, ICommand*> m_Cmds;
private:
    void ExecuteCommand(std::string commandName, CmdArgs& args);
};

extern CommandManager* g_commandManager;