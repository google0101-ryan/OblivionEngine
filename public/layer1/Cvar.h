#pragma once

#include <appsystem/AppSystem.h>

#define CVAR_INTERFACE_NAME "CvarSysV001"

class ICVar
{
public:
    virtual void Set(const char* value) = 0;
    virtual const char* GetString() const = 0;
    virtual int GetInt() const = 0;
    virtual float GetFloat() const = 0;
    virtual bool GetBool() const = 0;
};

abstract_class ICvarSystem : public IAppSystem
{
    DECLARE_APPSYSTEM( IAppSystem, ICvarSystem );
public:
    // DO NOT MODIFY CVARS
    // The only thing that should modify them is user-specified overrides on startup, which are handled WAY ahead of you
    virtual const ICVar* GetVar(const char* name, const char* value = "0", bool create = true) = 0;
    // This is used by the engine to register "+set" overrides, so that way we can automatically override cvars without references to the cvar system everywhere
    virtual void SetVarOverride(const char* pVarName, const char* pVarOverride) = 0;
};