#pragma once

#include <app/App.h>

// Systems
#include <layer1/Command.h>
#include <layer1/EventSystem.h>

typedef enum
{
    ERROR_STAGE_NONE = -1,
    ERROR_STAGE_CREATE_SYSTEMS
} ErrorStage_t;

class CEngineApp : public CBasicApp
{
public:
    virtual bool Create();
    virtual void Main();
    virtual void Shutdown();
private:
    bool CreateSystems();
    
    bool CreateLayer1Systems();

private:
    CreateSystemFactory_t LoadModule( const char* pModuleName );
private:
    ErrorStage_t m_errorStage;

    ICommandSystem* m_pCommandSystem;
    IEventSystem* m_pEventSystem;
};