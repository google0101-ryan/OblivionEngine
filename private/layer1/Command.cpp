#include <layer1/Command.h>
#include <appsystem/interface.h>

class CCommandSystem : public ICommandSystem
{
public:
    virtual bool Create();
    virtual void Destroy();

    virtual void AddCommand( CString name, CommandFunc_t pFunc );
private:
    std::unordered_map<const char*, CommandFunc_t> m_pFuncs;
};

CCommandSystem g_cmdSystem;

EXPOSE_INTERFACE_GLOBALVAR( ICommandSystem, g_cmdSystem, CMD_SYSTEM_INTERFACE_NAME );

bool CCommandSystem::Create()
{
    m_pFuncs.clear();

    return true;
}

void CCommandSystem::Destroy()
{
    m_pFuncs.clear();
}

void CCommandSystem::AddCommand(CString name, CommandFunc_t pFunc)
{
    m_pFuncs[name.GetCStr()] = pFunc;
}
