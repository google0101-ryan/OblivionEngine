#pragma once

#include <Framework/Events/Event.h>

class EventManager
{
public:
    void BeginFrame();

    void PushEvent(Event& event);
    Event& PopEvent();
private:
    Event m_Events[256];
    int m_iEventPushIndex;
    int m_iEventPopIndex;
};