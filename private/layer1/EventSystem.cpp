#include <layer1/EventSystem.h>
#include <appsystem/interface.h>

#define MAX_EVENTS_PER_FRAME 50

class CEventSystem : public IEventSystem
{
public:
    virtual bool Create();
    virtual void Destroy();

    virtual void BeginFrame();

    virtual void PushEvent( Event_t evt );
    virtual int GetEventCount();
    virtual Event_t& GetEventAtIndex( int index );
private:
    Event_t m_events[MAX_EVENTS_PER_FRAME];
    int m_eventCount;
};

CEventSystem g_eventSystem;
EXPOSE_INTERFACE_GLOBALVAR( IEventSystem, g_eventSystem, EVENT_SYSTEM_INTERFACE_NAME );

bool CEventSystem::Create()
{
    m_eventCount = 0;

    return true;
}

void CEventSystem::Destroy()
{
}

void CEventSystem::BeginFrame()
{
    m_eventCount = 0;
}

void CEventSystem::PushEvent(Event_t evt)
{
    m_events[m_eventCount++] = evt;
}

int CEventSystem::GetEventCount()
{
    return m_eventCount;
}

Event_t &CEventSystem::GetEventAtIndex(int index)
{
    return m_events[m_eventCount];
}
