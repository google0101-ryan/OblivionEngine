#pragma once

#include <app/App.h>

bool CBasicApp::Run()
{
    if (!Create())
    {
        printf( "CBasicApp::Create() failed\n" );
        return false;
    }

    m_bIsRunning = true;
    while (m_bIsRunning)
    {
        Main();
    }

    Shutdown();
    
    return true;
}