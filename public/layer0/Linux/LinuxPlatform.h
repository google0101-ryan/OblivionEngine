#pragma once

// DLL handling
#include <dlfcn.h>

#define DLL_EXT ".so"

typedef void* DllHandle_t;

#define DllOpen( x ) dlopen( x, RTLD_NOW )
#define DllSym( x, name ) dlsym( x, name )

#define DLL_EXPORT extern "C" __attribute__((visibility("default")))

// Threading
#include <pthread.h>

typedef void (*xthread_t)(void* pArg);

class [[nodiscard]] CSysThread
{
public:
    void StartThread( xthread_t threadFunc );

    virtual void Run() = 0;
private:
    static void* ThreadProc( void* pThread );

    pthread_t m_threadId;
};

inline void CSysThread::StartThread( xthread_t threadFunc )
{
    pthread_create( &m_threadId, nullptr, ThreadProc, this );
    pthread_detach( m_threadId );
}

inline void* CSysThread::ThreadProc( void *pThread )
{
    while (true)
        ((CSysThread*)pThread)->Run();
}
