#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <appsystem/AppSystem.h>

class CAppSystemGroup
{
public:
    CAppSystemGroup();

    void AddSystem(const char* pName, IAppSystem* pAppSystem);
    IAppSystem* GetSystem(const char* pName);

    void ConnectAll();
    bool InitAll();
    void ShutdownAll();

    SystemFactoryFn_t GetFactory();
private:
    std::unordered_map<std::string, IAppSystem*> m_pAppSystems;
    std::vector<IAppSystem*> m_pAppSystemList; // Needed for in-order initialization
};