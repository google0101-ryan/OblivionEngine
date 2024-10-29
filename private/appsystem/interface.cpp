#include <appsystem/interface.h>

static CInterfaceReg* s_pRegs = nullptr;

void* GetInterface( const char* pName )
{
    CInterfaceReg* pReg = s_pRegs;

    for ( ; pReg; pReg = pReg->m_pNext )
    {
        if ( !strcmp( pReg->m_pName, pName ) )
            return pReg->m_pFunc();
    }

    return nullptr;
}

CInterfaceReg::CInterfaceReg(InstantiateFunc_t pFunc, const char *pName)
{
    m_pFunc = pFunc;
    m_pName = pName;
    m_pNext = s_pRegs;
    s_pRegs = this;
}
