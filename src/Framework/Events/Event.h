#pragma once

enum EventType
{
    EVENT_KEY_PRESS,
    EVENT_KEY_HOLD,
    EVENT_KEY_RELEASE,
    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_CLICK
};

struct Event
{
    EventType m_type;

    struct
    {
        int m_iKeyCode;
        bool m_bPressed;
        bool m_bHeld;
    } m_keyData;

    struct
    {
        int m_iMouseX;
        int m_iMouseY;
        int i_MouseButton; // 1, 2, 3 for left, right, and middle
    } m_MouseData;
};