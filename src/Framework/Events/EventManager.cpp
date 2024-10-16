#include <Framework/Events/EventManager.h>

#include <Util/Logger.h>

void EventManager::BeginFrame()
{
    m_iEventPopIndex = 0;
    m_iEventPushIndex = 0;
}

void EventManager::PushEvent(Event &event)
{
    m_Events[m_iEventPushIndex] = event;
    if (++m_iEventPushIndex >= 255)
    {
        Logger::Log(LogLevel::WARNING, "Overflow in event manager on push");
        m_iEventPushIndex = 0;
    }
}

Event &EventManager::PopEvent()
{
    auto& event = m_Events[m_iEventPopIndex];
    if (++m_iEventPopIndex >= 255)
    {
        Logger::Log(LogLevel::WARNING, "Overflow in event manager on pop");
        m_iEventPopIndex = 0;
    }

    return event;
}
