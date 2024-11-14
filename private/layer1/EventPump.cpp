#include <layer1/EventPump.h>
#include <layer1/layer1.h>

#define MAX_EVENTS 256

class CEventManager : public CLayer1AppSystem<IEventManager>
{
public:
    virtual bool Init() { ResetFrame(); return true; }
    virtual void Shutdown() {}

    virtual void ResetFrame();
    virtual void PushEvent(EventType_t ev, int evData);
    virtual int GetEventCount() const;
    virtual const SEvent& GetEventAt(int index) const;
private:
    SEvent m_events[MAX_EVENTS];
    int m_eventIndex;
};

CEventManager g_eventManager;
EXPOSE_INTERFACE_GLOBALVAR(CEventManager, IEventManager, g_eventManager, EVENT_INTERFACE_NAME);

void CEventManager::ResetFrame()
{
    m_eventIndex = 0;
}

void CEventManager::PushEvent(EventType_t ev, int evData)
{
    m_events[m_eventIndex].m_eventType = ev;
    m_events[m_eventIndex++].m_eventData = evData;
    if (m_eventIndex >= MAX_EVENTS)
    {
        printf("WARNING: overflow in event manager!\n");
        m_eventIndex = 0;
    }
}

int CEventManager::GetEventCount() const
{
    return m_eventIndex;
}

const SEvent &CEventManager::GetEventAt(int index) const
{
    if (index >= m_eventIndex)
    {
        printf("ERR: Out of range access in event manager (%d:%d)\n", index, m_eventIndex);
        exit(1);
    }

    return m_events[index];
}
