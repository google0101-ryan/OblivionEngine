#pragma once

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdio>
#include <cctype>

#ifdef __linux__
#define PLATFORM_LINUX 1
#define PLATFORM_STRING "linux"
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define PLATFORM_WIN32 1
#define PLATFORM_STRING "win32"
#elif __APPLE__ && defined(TARGET_OS_MAC)
#define PLATFORM_MACOSX 1
#define PLATFORM_STRING "macos"
#else
#error Unsupported platform!
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define CPU_STRING "amd64"
#elif defined(i386) || defined(__i386__) || defined(_M_IX86)
#define CPU_STRING "x86"
#else
#define CPU_STRING "unknown"
#endif

#if defined(PLATFORM_LINUX)

#include <layer0/Linux/LinuxPlatform.h>

#endif