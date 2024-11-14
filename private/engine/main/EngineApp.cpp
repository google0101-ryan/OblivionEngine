#include <engine/main/EngineApp.h>
#include <layer1/Cvar.h>
#include <layer1/Command.h>
#include <layer1/EventPump.h>
#include <layer2/Window.h>
#include <layer2/FileSystem.h>
#include <layer0/layer0.h>
#include <cstring>

#define SET_CURDLL(x) \
    handle = DllOpen("bin/" DllName(x)); \
    if (!handle) \
    { \
        fprintf(stderr, "Failed to load library '" x "': %s\n", DllError()); \
        exit(EXIT_FAILURE); \
    }

#define LOAD_SYSTEM_CURDLL( ifaceName ) \
    { \
        printf("Loading " STRINGIFY(ifaceName) "\n"); \
        SystemFactoryFn_t pFactory = (SystemFactoryFn_t)DllSym(handle, "GetSystem"); \
        if (pFactory == nullptr) \
        { \
            fprintf(stderr, "Failed to load library: %s\n", DllError()); \
            exit(EXIT_FAILURE); \
        } \
        IAppSystem* pAppSystem = (IAppSystem*)pFactory( ifaceName ); \
        if (!pAppSystem) \
        { \
            fprintf(stderr, "Failed to find app system '%s'\n", ifaceName); \
            exit(EXIT_FAILURE); \
        } \
        m_appSystemGroup.AddSystem( ifaceName, pAppSystem ); \
    }

void CEngineApp::SetArgs(int argc, char **ppArgv)
{
    m_argc = argc;
    m_ppArgv = ppArgv;
}

void CEngineApp::Init()
{
    // Load liblayer1.so, which contains the cvar and command systems
    DllHandle_t handle;

    SET_CURDLL("layer1");

    LOAD_SYSTEM_CURDLL( COMMAND_INTERFACE_NAME );
    LOAD_SYSTEM_CURDLL( CVAR_INTERFACE_NAME );
    LOAD_SYSTEM_CURDLL(EVENT_INTERFACE_NAME);

    // TODO: Is this the best way to do this?
    // Parse the command line and find any "+set" commands, then register them in the cvar system
    // also TODO: Should we route these through the command system?
    ProcessEarlyVars();

    SET_CURDLL("layer2");

    LOAD_SYSTEM_CURDLL(WINDOW_INTERFACE_NAME);
    LOAD_SYSTEM_CURDLL(FILESYSTEM_INTERFACE_NAME);

    // Let everything know about whatever they need to know about
    m_appSystemGroup.ConnectAll();

    if (!m_appSystemGroup.InitAll())
    {
        printf("Failed to initialize systems!\n");
        exit(1);
    }
}

void CEngineApp::Main()
{
    auto pWindow = (IWindow*)m_appSystemGroup.GetSystem(WINDOW_INTERFACE_NAME);
    auto pEventManager = (IEventManager*)m_appSystemGroup.GetSystem(EVENT_INTERFACE_NAME);

    pEventManager->ResetFrame();

    pWindow->PumpEvents();

    for (int i = 0; i < pEventManager->GetEventCount(); i++)
    {
        auto ev = pEventManager->GetEventAt(i);

        switch (ev.m_eventType)
        {
        case ET_EXIT:
            m_isRunning = false;
            break;
        default:
            printf("Cannot handle unknown event %d\n", ev.m_eventType);
            break;
        }
    }
}

void CEngineApp::Shutdown()
{
    m_appSystemGroup.ShutdownAll();
}

void CEngineApp::ProcessEarlyVars()
{
    auto pCvarSystem = (ICvarSystem*)m_appSystemGroup.GetSystem(CVAR_INTERFACE_NAME);

    for (int i = 0; i < m_argc; i++)
    {
        if (!strcmp(m_ppArgv[i], "+set"))
        {
            if (i+2 >= m_argc)
            {
                printf("Malformed command line!\n");
                return;
            }

            pCvarSystem->SetVarOverride(m_ppArgv[i+1], m_ppArgv[i+2]);

            printf("Added override for var \"%s\": \"%s\"\n", m_ppArgv[i+1], m_ppArgv[i+2]);
            i += 2;
        }
    }
}
