#include <layer2/Window.h>
#include <layer2/layer2.h>
#include <SDL3/SDL.h>

class CWindow : public CLayer2AppSystem<IWindow>
{
public:
    virtual bool Init();
    virtual void Shutdown();

    virtual void PumpEvents();

    virtual int GetWidth() const { return m_pWidth->GetInt(); }
    virtual int GetHeight() const { return m_pHeight->GetInt(); }
private:
    SDL_Window* m_pWindow;
    const ICVar* m_pWidth;
    const ICVar* m_pHeight;
};

CWindow g_window;
EXPOSE_INTERFACE_GLOBALVAR(CWindow, IWindow, g_window, WINDOW_INTERFACE_NAME);

bool CWindow::Init()
{
    auto pCvarSys = GetCvarSystem();
    if (!pCvarSys)
    {
        printf("Failed to get cvar system\n");
        return false;
    }

    m_pWidth = pCvarSys->GetVar("r_windowWidth", "1024");
    m_pHeight = pCvarSys->GetVar("r_windowHeight", "768");

    printf("Window is %dx%d\n", m_pWidth->GetInt(), m_pHeight->GetInt());

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("Failed to init SDL3: %s\n", SDL_GetError());
        return false;
    }

    m_pWindow = SDL_CreateWindow( "App", m_pWidth->GetInt(), m_pHeight->GetInt(), 0 );

    if (!m_pWindow)
    {
        printf("Failed to init window: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void CWindow::Shutdown()
{
    SDL_DestroyWindow(m_pWindow);
    SDL_Quit();
}

void CWindow::PumpEvents()
{
    auto pEventManager = GetEventManager();

    if (!pEventManager)
    {
        printf("Failed to get event manager!\n");
        return;
    }

    SDL_Event ev;
    while (SDL_PollEvent(&ev))
    {
        switch (ev.type)
        {
        case SDL_EVENT_QUIT:
            pEventManager->PushEvent(ET_EXIT, 0);
            break;
        }
    }
}
