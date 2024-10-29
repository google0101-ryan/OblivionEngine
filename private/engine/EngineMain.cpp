#include <layer0/layer0.h>
#include <engine/Version.h>
#include <engine/main/EngineApp.h>

DLL_EXPORT void EngineMain( int argc, char** argv )
{
    printf( "%s\n", GetVersion() );

    CEngineApp app;
    
    if (!app.Run())
    {
        printf( "Engine run failed\n" );
    }

    return;
}