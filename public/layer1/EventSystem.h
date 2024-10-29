#pragma once

#include <appsystem/AppSystem.h>

typedef enum EventType_t
{
    EV_EXIT,
    EV_KEY_PRESS,
    EV_KEY_HOLD,
    EV_KEY_RELEASE,
    EV_MOUSE_MOVE
};

struct Event_t
{
    EventType_t m_evType;
    float m_floatArg;
    int m_intArg;
    void* m_pArg;
};

#define EVENT_SYSTEM_INTERFACE_NAME "EventSysV001"
abstract_class IEventSystem : public CLayer1AppSystem<IAppSystem>
{
public:
    virtual void BeginFrame() = 0;

    virtual void PushEvent( Event_t evt ) = 0;
    virtual int GetEventCount() = 0;
    virtual Event_t& GetEventAtIndex( int index ) = 0;
};