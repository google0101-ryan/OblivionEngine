#include <Framework/Entities/Components/Renderable.h>
#include "Framework/Entities/Entity.h"
#include "Framework/Platform/Linux/Linux.inl"
#include <Framework/Engine/Engine.h>

#include <Framework/Platform/Platform.h>
#include <Framework/Cvar/Cvar.h>
#include <Framework/Command/Command.h>
#include <Framework/Window/Window.h>
#include <Framework/Filesystem/FileSystem.h>

#include <cassert>

struct EngineState
{
    Window* m_pWindow;
    EventManager* m_pEventManager;
    RenderFrontend* m_pFrontend;

    bool m_bIsRunning = false;
} g_engineState;

void ProcessEarlyCvars(int argc, const char** argv)
{
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "+set") == 0)
        {
            if (i+2 >= argc)
                return;
            std::string cmdLineString = std::string(argv[i]+1) + " " + argv[i + 1] + " " + argv[i + 2];
            g_commandManager->ExecuteCommand(cmdLineString);
        }
    }
}

void Engine::Init(EngineConfig& config)
{
    Sys_Milliseconds();

    // Set up cvars so we can have configs from the user
    CVar::Initialize();

    // Get any "+set" commands off of the command line ASAP
    ProcessEarlyCvars(config.iArgc, config.ppArgv);

    // Sanity test
    assert(CVar::GetVar("cheats", false));

    // Now that cvars are working, we can do fs initialization
    g_pFilesystem->Initialize();

    // Initialize window so we can actually see things
    g_engineState.m_pWindow = new Window(config.iWidth, config.iHeight, config.pszTitle, config.bFullscreen);

    // Handles events pumped by the window, such as key presses
    g_engineState.m_pEventManager = new EventManager();

    g_engineState.m_pFrontend = new RenderFrontend();
    g_engineState.m_pFrontend->Init();

    // Add the entity manager to the update list
    g_EntityManager->Init();

    auto exampleEntity = new Entity("example");
    exampleEntity->AddComponent("renderMesh", new Renderable());

    // Once all core systems have been initialized, load the game-specific DLL/.so file

    g_engineState.m_bIsRunning = true;
}

void Engine::Tick()
{
    g_engineState.m_pEventManager->BeginFrame();
    g_engineState.m_pFrontend->BeginFrame();

    g_engineState.m_pWindow->PumpEvents();

    if (!g_engineState.m_bIsRunning)
        this->Exit(0);

    // Engine state updated. Delegate updates to systems that need them that aren't core systems
    // This includes AI, game-specific state, etc
    for (auto& listener : tickListeners)
    {
        listener();
    }

    g_engineState.m_pFrontend->DrawWorld();
    
    g_engineState.m_pWindow->Update();
}

bool Engine::IsRunning()
{
    return g_engineState.m_bIsRunning;
}

void Engine::RequestShutdown()
{
    g_engineState.m_bIsRunning = false;
}

void Engine::Exit(int iCode)
{
    g_engineState.m_pFrontend->Exit();

    PlatExit(iCode);
}

void Engine::AddTickListener(TickListener listener)
{
    tickListeners.push_back(listener);
}

void Engine::AddFixedTickListener(TickListener listener)
{
    fixedTickListeners.push_back(listener);
}

EventManager* Engine::GetEventManager()
{
    return g_engineState.m_pEventManager;
}

Window *Engine::GetMainWindow()
{
    return g_engineState.m_pWindow;
}

Engine localEngine;
Engine* g_pEngine = &localEngine;