#include <layer1/Cvar.h>
#include <layer1/Command.h>
#include <layer1/layer1.h>
#include <string>
#include <unordered_map>

class CCVar : public ICVar
{
public:
    CCVar(const char* pName, const char* pValue)
    : m_pName(pName)
    {
        Set( pValue );
    }

    virtual void Set(const char* value);
    virtual const char* GetString() const;
    virtual int GetInt() const;
    virtual float GetFloat() const;
    virtual bool GetBool() const;
private:
    const char* m_pName;
    const char* m_pValue;
    int m_iValue;
    float m_fValue;
};

void CCVar::Set(const char *value)
{
    m_pValue = value;
    m_iValue = atoi(m_pValue);
    m_fValue = atof(m_pValue);
}

const char *CCVar::GetString() const
{
    return m_pValue;
}

int CCVar::GetInt() const
{
    return m_iValue;
}

float CCVar::GetFloat() const
{
    return m_fValue;
}

bool CCVar::GetBool() const
{
    return (bool)m_iValue;
}

class CCvarSystem : public CLayer1AppSystem<ICvarSystem> 
{
    DECLARE_APPSYSTEM( CLayer1AppSystem<ICvarSystem>, CCvarSystem );
public:
    virtual bool Init();
    virtual void Connect(SystemFactoryFn_t pFactory) override;
    virtual void Shutdown() {}

    // Each module that wishes to use cvars must call this function in Init()
    // i.e. the renderer might have a var, r_fullscreen
    // it would have to call cmdSystem->GetVar("r_fullscreen", "0");
    virtual const ICVar* GetVar(const char* name, const char* value = "0", bool create = true);
    // Hidden non-const version for internal use
    ICVar* GetVarInternal(const char* name, const char* value = "0", bool create = true);
    virtual void SetVarOverride(const char* pVarName, const char* pVarOverride);
private:
    static void Set(CmdArgs_t& args);

    ICommandSystem* m_pCmdSystem; // Store this reference for use during Init()
    std::unordered_map<std::string, ICVar*> m_pCvars;
    std::unordered_map<std::string, std::string> m_pOverrides;
};

CCvarSystem g_cvarSystem;
EXPOSE_INTERFACE_GLOBALVAR( CCvarSystem, ICvarSystem, g_cvarSystem, CVAR_INTERFACE_NAME );

bool CCvarSystem::Init()
{
    // Don't fail if we somehow couldn't find the command system
    if (m_pCmdSystem)
    {
        m_pCmdSystem->AddCommand("set", Set);
    }

    GetVar("g_cheats", "0");

    return true;
}

void CCvarSystem::Connect(SystemFactoryFn_t pFactory)
{
    // We need a reference to the command system for the +set command
    m_pCmdSystem = (ICommandSystem*)pFactory( COMMAND_INTERFACE_NAME );
    if (!m_pCmdSystem)
    {
        printf("Failed to find command system! Cvar commands (+set) will be unavailable\n");
    }
}

const ICVar *CCvarSystem::GetVar(const char *pName, const char *value, bool create)
{
    return GetVarInternal(pName, value, create);
}

ICVar *CCvarSystem::GetVarInternal(const char *pName, const char *value, bool create)
{   
    if (m_pCvars.find(pName) == m_pCvars.end())
    {
        if (!create)
            return nullptr;

        ICVar* pCvar = new CCVar(pName, value);
        m_pCvars[pName] = pCvar;

        if (m_pOverrides.find(pName) != m_pOverrides.end())
        {
            pCvar->Set(m_pOverrides[pName].c_str());
        }

        return pCvar;
    }

    return m_pCvars[pName];
}

void CCvarSystem::SetVarOverride(const char *pVarName, const char *pVarOverride)
{
    m_pOverrides[pVarName] = pVarOverride;
}

void CCvarSystem::Set(CmdArgs_t &args)
{
    if (args.size() < 3)
    {
        printf("Usage: %s <cvar> <value>\n", args[0].c_str());
        return;
    }

    auto var = g_cvarSystem.GetVarInternal( args[1].c_str(), args[2].c_str(), false );
    if (!var)
        printf("Tried to set cvar '%s' but it doesn't exist\n", args[1].c_str());
    var->Set( args[2].c_str() );
}
