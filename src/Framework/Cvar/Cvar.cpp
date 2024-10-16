#include "Cvar.h"
#include <Framework/Command/Command.h>

#define MODULE "CVAR"
#include <Util/Logger.h>

class CvarSetCommand : public ICommand
{
public:
    virtual void Execute(CmdArgs& args) override;
};

CvarSetCommand g_cvarSetCommand;

CVar* CVar::s_pCvars = NULL;
std::unordered_map<std::string, CVar*> CVar::s_cvars;

static CVar g_cheats("cheats", "0", CVAR_FLAG_DEFAULT);

CVar::CVar(std::string name, std::string initValue, int flags)
{
    m_Name = name;
    m_ResetValue = initValue;
    m_Value = initValue;
    m_fValue = atof(initValue.c_str());
    m_iValue = atoi(initValue.c_str());
    m_Flags = flags;

    m_pNext = s_pCvars;
    s_pCvars = this;
}

void CVar::Set(std::string &value)
{
    if (m_Flags & CVAR_FLAG_ROM)
    {
        Logger::Log(LogLevel::WARNING, "Cannot set read-only cvar '%s'", m_Name.c_str());
        return;
    }

    if ((m_Flags & CVAR_FLAG_CHEAT) && g_cheats.m_iValue != 1)
    {
        Logger::Log(LogLevel::WARNING, "Cannot set cheats-only cvar '%s' with cheats disabled", m_Name.c_str());
        return;
    }

    m_Value = value;
    m_fValue = atof(value.c_str());
    m_iValue = atoi(value.c_str());

    Logger::Log(LogLevel::DEBUG, "Set '%s' to '%s'", m_Name.c_str(), value.c_str());
}

void CVar::Initialize()
{
    auto cvar = s_pCvars;
    for (; cvar; cvar = cvar->m_pNext)
    {
        s_cvars[cvar->m_Name] = cvar;
    }

    g_commandManager->AddCommand("set", &g_cvarSetCommand);
}

CVar *CVar::GetVar(std::string name, bool create, std::string value, int flags)
{
    // Search for cvar
    if (s_cvars.find(name) == s_cvars.end())
    {
        if (!create)
            return nullptr;
        // Create it
        CVar* var = new CVar(name, value, flags);
        s_cvars[name] = var;
        return var;
    }

    return s_cvars[name];
}

// CVAR COMMANDS

void CvarSetCommand::Execute(CmdArgs &args)
{
    if (args.argc < 2)
    {
        Logger::Log(LogLevel::INFO, "Usage: set <cvar> <value>");
        return;
    }

    // Find variable
    auto cvar = CVar::GetVar(args.argv[0], false);
    if (!cvar)
    {
        Logger::Log(LogLevel::WARNING, "Failed to set non-existant cvar '%s'", args.argv[0].c_str());
        return;
    }

    cvar->Set(args.argv[1]);
    return;
}
