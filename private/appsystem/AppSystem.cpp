#include <appsystem/AppSystem.h>
#include <cstring>

static InterfaceReg_t* s_pRegs = nullptr;

void* GetSystem_Internal(const char* pName)
{
    auto pReg = s_pRegs;
    for (; pReg; pReg = pReg->m_pNext)
    {
        if (strcmp(pReg->m_pName, pName) == 0)
            return pReg->m_pFn();
    }
    return nullptr;
}

// Try to get an app system from this DLL
void* GetSystem(const char* pName)
{
    return GetSystem_Internal( pName );
}

InterfaceReg_t::InterfaceReg_t(const char *pName, CreateSystemFn_t pFn)
{
    m_pName = pName;
    m_pFn = pFn;

    m_pNext = s_pRegs;
    s_pRegs = this;
}
