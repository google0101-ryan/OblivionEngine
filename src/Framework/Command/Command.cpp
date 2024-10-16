#include "Command.h"
#define MODULE "CMD"
#include <Util/Logger.h>

void CommandManager::AddCommand(std::string cmdName, ICommand *cmd)
{
    m_Cmds[cmdName] = cmd;
}

void CommandManager::ExecuteCommand(std::string commandText)
{
    // Split command line into command + args seperated by semicolons
    // i.e. "set cheats 1; bind shoot mouse_1"
    int index = 0;
    while (commandText[index])
    {
        CmdArgs args;

        // Skip whitespace
#define SKIPWHITESPACE \
        while (commandText[index] <= ' ') \
        { \
            index++; \
            if (!commandText[index]) \
                goto done; \
        } \

        // Skip whitespace manually because we need a command name to run
        while (commandText[index] <= ' ')
        {
            index++;
            if (!commandText[index])
                return;
        }
        std::string cmdName;
        while (commandText[index] > ' ')
        {
            cmdName.push_back(commandText[index]);
            index++;
        }
        SKIPWHITESPACE;
        // Add arguments
        while (commandText[index] && commandText[index] != ';')
        {
            args.argc++;
            SKIPWHITESPACE;
            std::string arg;
            while (commandText[index] > ' ')
            {
                arg.push_back(commandText[index]);
                index++;
            }
            args.argv.push_back(arg);
            SKIPWHITESPACE;
        }
done:
        ExecuteCommand(cmdName, args);
    }
}

void CommandManager::ExecuteCommand(std::string commandName, CmdArgs &args)
{
    if (m_Cmds.find(commandName) == m_Cmds.end())
    {
        Logger::Log(LogLevel::WARNING, "Failed to execute command: No such command '%s'", commandName.c_str());
        return;
    }

    m_Cmds[commandName]->Execute(args);
}

CommandManager localCommandManager;
CommandManager* g_commandManager = &localCommandManager;