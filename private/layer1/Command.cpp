#include <layer1/Command.h>
#include <layer1/layer1.h>
#include <layer0/layer0.h>
#include <layer0/Lexer.h>
#include <unordered_map>
#include <cassert>

class CCommandSystem : public CLayer1AppSystem<ICommandSystem>
{
    DECLARE_APPSYSTEM( CLayer1AppSystem<ICommandSystem>, CCommandSystem );
public:
    virtual bool Init();
    virtual void Shutdown();

    virtual void AddCommand(const char* pCmdName, Command_t pCommand);
    virtual void ExecuteCommand(const char* pCmdText);
private:
    std::unordered_map<std::string, Command_t> m_pCmds;
};

CCommandSystem g_cmdSystem;
EXPOSE_INTERFACE_GLOBALVAR( CCommandSystem, ICommandSystem, g_cmdSystem, COMMAND_INTERFACE_NAME );

bool CCommandSystem::Init()
{
    // TODO: Adjust this. We're probably *way* overdoing it
    m_pCmds.reserve( 1024 );
    return true;
}

void CCommandSystem::Shutdown()
{
    m_pCmds.clear();
}

void CCommandSystem::AddCommand(const char *pCmdName, Command_t pCommand)
{
    printf("Registering command: %s\n", pCmdName);
    m_pCmds[pCmdName] = pCommand;
}

void CCommandSystem::ExecuteCommand(const char *pCmdText)
{
    CLexer lex(pCmdText);
    CmdArgs_t args;

    SToken tok;
    if (!lex.GetToken(&tok))
    {
        printf("Invalid command passed to ExecuteCommand()\n");
        return;
    }

    assert(tok.m_tokenType == TT_NAME);
    args[0] = tok.m_value;

    int index = 1;
    while (lex.GetToken(&tok))
    {
        args[index++] = tok.m_value;
    }

    if (m_pCmds.find(args[0].c_str()) == m_pCmds.end())
    {
        printf("Failed to execute command '%s' because it does not exist\n", args[0].c_str());
        return;
    }

    m_pCmds[args[0].c_str()](args);
}
