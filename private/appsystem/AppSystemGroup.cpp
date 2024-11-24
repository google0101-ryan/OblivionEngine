#include <appsystem/AppSystemGroup.h>

static CAppSystemGroup* s_pCurAppSystem;

CAppSystemGroup::CAppSystemGroup()
{
    s_pCurAppSystem = this;
    m_pAppSystems.reserve(10); // Stave off allocations for a bit
}

void CAppSystemGroup::AddSystem(const char *pName, IAppSystem *pAppSystem)
{
    m_pAppSystems[pName] = pAppSystem;
    m_pAppSystemList.push_back(pAppSystem);
}

IAppSystem *CAppSystemGroup::GetSystem(const char *pName)
{
    IAppSystem* ret = nullptr;
    try
    {
        ret = m_pAppSystems.at(pName);
        return ret;
    }
    catch (std::exception& e)
    {
        printf("Failed to find system: %s\n", pName);
        for (auto& sys : m_pAppSystems)
            printf("\t%s\n", sys.first);
        return nullptr;
    }
}

void CAppSystemGroup::ConnectAll()
{
    for (auto& pSystem : m_pAppSystems)
        pSystem.second->Connect(GetFactory());
}

bool CAppSystemGroup::InitAll()
{
    for (auto& pSystem : m_pAppSystemList)
    {
        // It's up to the systems themselves to provide more verbose error messages
        if (!pSystem->Init())
            return false;
    }

    return true;
}

void CAppSystemGroup::ShutdownAll()
{
    for (auto& pSystem : m_pAppSystems)
    {
        pSystem.second->Shutdown();
    }
}

void* AppSystemGroupFactory(const char* pName)
{
    return s_pCurAppSystem->GetSystem(pName);
}

SystemFactoryFn_t CAppSystemGroup::GetFactory()
{
    return AppSystemGroupFactory;
}
