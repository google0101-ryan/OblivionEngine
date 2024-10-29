#include <layer0/layer0.h>

// MAYBE TODO: Expand this to include a graphical launcher window?

typedef void (*EngineMain_t)(int, char**);

int main(int argc, char** argv)
{
    // Just load engine.so for now
    CString path("bin/libengine.so");

    DllHandle_t pHandle = DllOpen( path.GetCStr() );

    if ( !pHandle )
    {
        printf( "Failed to load libengine.so: %s\n", dlerror() );
        return 1;
    }

    EngineMain_t engineMain = (EngineMain_t)DllSym( pHandle, "EngineMain" );

    if ( !engineMain )
    {
        printf( "Failed to load libengine.so: %s\n", dlerror() );
        return 1;
    }

    engineMain(argc, argv);

    return 0;
}