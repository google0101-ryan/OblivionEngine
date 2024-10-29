#pragma once

#include <layer0/layer0.h>

#define ENGINE_VER_MAJOR 1
#define ENGINE_VER_MINOR 0
#define ENGINE_VER_PATCH 0

#define _STRINGIFY( x ) #x
#define STRINGIFY( x ) _STRINGIFY( x )

inline const char* GetVersion()
{
    return "Oblivion v" STRINGIFY( ENGINE_VER_MAJOR ) "."
                        STRINGIFY( ENGINE_VER_MINOR ) "."
                        STRINGIFY( ENGINE_VER_PATCH ) " "
                        PLATFORM_STRING " "
                        CPU_STRING;
}