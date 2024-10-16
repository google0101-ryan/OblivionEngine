#pragma once

#ifdef _WIN32
#define PLATFORM_WINDOWS
#elif defined(__linux__)
#define PLATFORM_LINUX
#elif defined(__FreeBSD__)
#define PLATFORM_BSD
#elif defined(__OSX__)
#define PLATFORM_APPLE
#endif

#if defined(PLATFORM_LINUX) || defined(PLATFORM_BSD) || defined(PLATFORM_APPLE)
#define PLATFORM_UNIX
#endif

