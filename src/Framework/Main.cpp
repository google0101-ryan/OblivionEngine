#include <Framework/Engine/Engine.h>

int main(int argc, const char** argv)
{
    // The defaults are fine for now
    EngineConfig config;
    config.iArgc = argc;
    config.ppArgv = argv;

    g_pEngine->Init(config);
    
    while (g_pEngine->IsRunning())
    {
        g_pEngine->Tick();
    }

    g_pEngine->Exit(0);
}