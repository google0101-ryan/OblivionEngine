#include <AppFramework/Application.h>

// Game implementation
// All games must declare an Application class
// And then export the app as a global variable using EXPORT_APPCLASS_MAIN(&g_app)

class GameApp : public IApplication
{
public:
    virtual void Init(AppCommunication* pAppComms);
    virtual void Tick();
    virtual void FixedTick();
    virtual void Shutdown();
};

EXPOSE_APPCLASS(GameApp);

Engine* g_pEngine;

void GameApp::Init(AppCommunication* pAppComms)
{
    g_pEngine = pAppComms->GetEngine();

    g_pEngine->AddTickListener(std::bind(&GameApp::Tick, this));
    g_pEngine->AddFixedTickListener(std::bind(&GameApp::FixedTick, this));
}

void GameApp::Tick()
{
    printf("Game tick!\n");
}

void GameApp::FixedTick()
{
}

void GameApp::Shutdown()
{
}
