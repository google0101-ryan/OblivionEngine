#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <pthread.h>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>
#include <chrono>
#include <thread>
#include <cstdio>

#define MAX_PATH PATH_MAX
#define PATH_SEPERATOR "/"
#define DEFAULT_BASEDIR ""
#define DEFAULT_SAVEDIR ".local/share/oblivion/"

#ifndef GAMEDIR_BASE
// Fallback default option
#define GAMEDIR_NAME "base"
#endif

inline void PlatExit(int iCode)
{
    exit(iCode);
}

inline void PlatSleepMS(int x)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(x));
}

extern uint64_t sys_timeBase;
inline uint64_t Sys_Milliseconds()
{
    int curtime;
    struct timespec ts;

	clock_gettime( CLOCK_MONOTONIC_RAW, &ts );

	if( !sys_timeBase )
	{
		sys_timeBase = ts.tv_sec;
		return ts.tv_nsec / 1000000;
	}

	curtime = ( ts.tv_sec - sys_timeBase ) * 1000 + ts.tv_nsec / 1000000;

	return curtime;
}

class SysThread
{
public:
    SysThread()
    {
        m_bExitRequested = false;
    }

    ~SysThread()
    {
        Stop();
    }

    void Start();
    void Run();

    void Stop();

    void SetName(const char* name)
    {
        pthread_setname_np(m_ThreadId, name);
    }
protected:
    pthread_t m_ThreadId;

    bool m_bExitRequested;
protected:
    virtual void DoWork() = 0;
    virtual void Shutdown() = 0;
};

inline void* pthread_start_func(void* data)
{
    ((SysThread*)data)->Run();

    return NULL;
}

inline void SysThread::Start()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_create(&m_ThreadId, &attr, pthread_start_func, (void*)this);
}

inline void SysThread::Run()
{
    while (!m_bExitRequested)
    {
        DoWork();
    }

    Shutdown();
}

inline void SysThread::Stop()
{
    m_bExitRequested = true;
}

inline char* Sys_GetBasePath()
{
    static char path[MAX_PATH] = {0};
    if (path[0])
        return path;
    getcwd(path, MAX_PATH);

    strncat(path, PATH_SEPERATOR, MAX_PATH);
    strncat(path, DEFAULT_BASEDIR, MAX_PATH);

    return path;
}

inline char* Sys_DefaultSavePath()
{
    static char path[MAX_PATH] = {0};
    if (path[0])
        return path;
    
    char* homePath = getenv("HOME");
    if (!homePath)
    {
        homePath = getpwuid(getuid())->pw_dir;
    }
    
    strncpy(path, homePath, MAX_PATH);
    strncat(path, PATH_SEPERATOR, MAX_PATH);
    strncat(path, DEFAULT_SAVEDIR, MAX_PATH);
    
    return path;
}