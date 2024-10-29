#pragma once

#include <layer0/layer0.h>

typedef void* (*InstantiateFunc_t)();

class CInterfaceReg
{
public:
    CInterfaceReg( InstantiateFunc_t pFunc, const char* pName );
public:
    InstantiateFunc_t m_pFunc;
    const char* m_pName;

    CInterfaceReg* m_pNext;
};

#define EXPOSE_INTERFACE_GLOBALVAR( ifaceName, gvarName, ifaceString ) \
    static void* __ ##ifaceName ##__Instantiate() { return static_cast<ifaceName*>( &gvarName ); } \
    static CInterfaceReg __ ##ifaceName ##__InterfaceReg( __ ##ifaceName ##__Instantiate, ifaceString );

DLL_EXPORT void* GetInterface( const char* pName );