#pragma once

#include <layer1/layer1.h>
#include <layer1/Cvar.h>
#include <layer1/Command.h>
#include <layer1/EventPump.h>

template<typename T>
class CLayer2AppSystem : public CLayer1AppSystem<T>
{
    typedef CLayer1AppSystem<T> BaseClass;
public:
    // Note that it's up to the app systems to make sure they have what they want
    virtual void Connect( SystemFactoryFn_t pFunc ) override
    {
        m_pCvarSystem = (ICvarSystem*)pFunc(CVAR_INTERFACE_NAME);
        m_pEventManager = (IEventManager*)pFunc(EVENT_INTERFACE_NAME);
        m_pCmdSystem = (ICommandSystem*)pFunc(COMMAND_INTERFACE_NAME);

        BaseClass::Connect(pFunc);
    }

    // Use this to force systems to be more explicit about their access
    ICvarSystem* GetCvarSystem()
    {
        return m_pCvarSystem;
    }

    IEventManager* GetEventManager()
    {
        return m_pEventManager;
    }

    ICommandSystem* GetCommandSystem()
    {
        return m_pCmdSystem;
    }
private:
    ICvarSystem* m_pCvarSystem;
    IEventManager* m_pEventManager;
    ICommandSystem* m_pCmdSystem;
};