#include <app/App.h>

void CBaseApplication::Run()
{
    Init();

    m_isRunning = true;
    while (m_isRunning)
    {
        Main();
    }

    Shutdown();
}