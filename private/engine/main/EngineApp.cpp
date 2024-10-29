#include <engine/main/EngineApp.h>
#include <appsystem/interface.h>

bool CEngineApp::Create()
{
    if ( !CreateSystems() )
    {
        return false;
    }

    return true;
}

void CEngineApp::Main()
{
    m_bIsRunning = false; // Temporary
}

void CEngineApp::Shutdown()
{
}

bool CEngineApp::CreateSystems()
{
    if ( !CreateLayer1Systems() )
    {
        return false;
    }

    return true;
}

#define LOAD_MODULE( modName ) \
    pFactory = LoadModule( modName ); \
    if ( !pFactory ) \
    { \
        printf( "Failed to get " modName "interface factory: %s\n", dlerror() ); \
        return false; \
    }

#define GET_APPSYSTEM( varName, varType, ifaceName ) \
    printf("-----\tInitializing " #varName "\t-----\n"); \
    varName = (varType*)pFactory( ifaceName ); \
    if ( !varName ) \
    { \
        printf( "Failed to create " #varName ": %s\n", dlerror() ); \
        return false; \
    } \
    varName->Create();

bool CEngineApp::CreateLayer1Systems()
{
    CreateSystemFactory_t pFactory;

    LOAD_MODULE( "layer1" );

    GET_APPSYSTEM( m_pCommandSystem, ICommandSystem, CMD_SYSTEM_INTERFACE_NAME );
    GET_APPSYSTEM( m_pEventSystem, IEventSystem, EVENT_SYSTEM_INTERFACE_NAME );
}

CreateSystemFactory_t CEngineApp::LoadModule(const char *pModuleName)
{
    CString modName( "bin/lib" );
    modName.Append( pModuleName );
    modName.Append( DLL_EXT );

    DllHandle_t modHandle = DllOpen( modName.GetCStr() );

    if ( !modHandle )
    {
        return nullptr;
    }

    CreateSystemFactory_t pFactory = (CreateSystemFactory_t)DllSym( modHandle, "GetInterface" );

    return pFactory;
}
