#pragma once

#include <appsystem/AppSystem.h>

typedef enum
{
    ET_EXIT,
    ET_KEY_PRESS,
    ET_KEY_HOLD,
    ET_KEY_RELEASE,
    ET_MOUSE_MOVE
} EventType_t;

struct SEvent
{
    EventType_t m_eventType;
    int m_eventData;
};

#define EVENT_INTERFACE_NAME "EventSysV001"
// mostly used by the window class to communicate SDL3 events in a platform-independent way
class IEventManager : public IAppSystem
{
public:
    virtual void ResetFrame() = 0;
    virtual void PushEvent(EventType_t ev, int evData) = 0;
    virtual int GetEventCount() const = 0;
    virtual const SEvent& GetEventAt(int index) const = 0;
};