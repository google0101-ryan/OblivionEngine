#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cstdarg>
#include <filesystem>

namespace fs
{
    using namespace std::filesystem;
};

#define STRINGIFY_HELPER(X) #X
#define STRINGIFY(X) STRINGIFY_HELPER(X)

#define abstract_class class

// DLL stuff
#ifdef __linux

#define PLATFORM_LINUX

#include <dlfcn.h>

typedef void* DllHandle_t;
#define DllOpen(path) dlopen(path, RTLD_NOW | RTLD_LOCAL)
#define DllClose(pHandle) dlclose(pHandle)
#define DllSym(pHandle, symName) dlsym(pHandle, symName)
#define DllError() dlerror()

#define DllVisible __attribute__((visibility("default")))

#define DLL_PREFIX "lib"
#define DLL_EXT ".so"

#define DllName(x) DLL_PREFIX x DLL_EXT

#else

#define PLATFORM_WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

typedef HMODULE DllHandle_t;
#define DllOpen(x) LoadLibrary(x)
#define DllClose(pHandle) FreeLibrary(pHandle)
#define DllSym(pHandle, symName) GetProcAddress(pHandle, symName)
#define DllError() std::to_string(GetLastError()).c_str()

#define DllVisible __declspec(dllexport)

#define DLL_PREFIX
#define DLL_EXT ".dll"

#define DllName(x) DLL_PREFIX x DLL_EXT

#endif