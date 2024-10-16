#pragma once

#include <string>
#include <unordered_map>

enum CVarFlags_t
{
    CVAR_FLAG_NONE = 0,
    CVAR_FLAG_ROM = 1<<0,
    CVAR_FLAG_CHEAT = 1<<1,
    CVAR_FLAG_ARCHIVE = 1<<2,
    CVAR_FLAG_SERVER = 1<<3,
    CVAR_FLAG_CLIENT = 1<<4
};

#define CVAR_FLAG_DEFAULT (CVAR_FLAG_CLIENT | CVAR_FLAG_SERVER)

class CVar
{
    friend class CvarSetCommand;
public:
    CVar(std::string name, std::string initValue, int flags);

    void Set(std::string& value);
    std::string& Get() {return m_Value;}
    bool GetBool() {return (bool)m_iValue;}
    int GetInt() {return m_iValue;}
public:
    // CVar management functions
    static void Initialize();
    static CVar* GetVar(std::string name, bool create = true, std::string value = "0", int flags = CVAR_FLAG_DEFAULT);
private:
    std::string m_Name;
    std::string m_Value;
    std::string m_ResetValue;
    double m_fValue;
    int m_iValue;

    int m_Flags;
private:
    static CVar* s_pCvars;
    CVar* m_pNext;
    static std::unordered_map<std::string, CVar*> s_cvars;
};