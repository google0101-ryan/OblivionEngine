#pragma once

#include <layer0/layer0.h>

typedef void* (*SystemFactoryFn_t)(const char*);
typedef void* (*CreateSystemFn_t)();

abstract_class IAppSystem
{
public:
    // Where we get to politely ask the parent app system group for knowledge of other app systems
    virtual void Connect( SystemFactoryFn_t pFactory ) = 0;
    // Initialization routine
    virtual bool Init() = 0;
    // Called on engine shutdown
    virtual void Shutdown() = 0;
};

#define DECLARE_APPSYSTEM( baseClass, thisClass ) \
    typedef baseClass BaseClass; \
    typedef thisClass ThisClass;

struct InterfaceReg_t
{
    InterfaceReg_t(const char* pName, CreateSystemFn_t pFn);

    const char* m_pName;
    CreateSystemFn_t m_pFn;

    InterfaceReg_t* m_pNext;
};

#define EXPOSE_INTERFACE_GLOBALVAR( className, ifaceClass, var, ifaceString ) \
    static void* __Create ##className ##ifaceClass ##_() { return static_cast<ifaceClass*>( &var ); } \
    static InterfaceReg_t __Iface ##className ##_Reg( ifaceString,  __Create ##className ##ifaceClass ##_)

extern "C" DllVisible void* GetSystem(const char* pName);