#pragma once

#include <app/App.h>
#include <appsystem/AppSystemGroup.h>

class CEngineApp : public CBaseApplication
{
public:
    void SetArgs(int argc, char** ppArgv);
    virtual void Init();
    virtual void Main();
    virtual void Shutdown();
private:
    void ProcessEarlyVars();
private:
    CAppSystemGroup m_appSystemGroup;
    int m_argc;
    char** m_ppArgv;
};