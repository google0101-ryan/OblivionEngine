#pragma once

#include <Framework/Engine/Engine.h>

// Header for communicating between game library and engine

class AppCommunication
{
public:
    Engine* GetEngine() {return g_pEngine;}
};