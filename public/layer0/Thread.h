#pragma once

#include <layer0/layer0.h>
#include <string>

#ifdef PLATFORM_LINUX

#include <pthread.h>
typedef pthread_t ThreadHandle_t;

#define MakeThread(threadHandle, entry, arg) pthread_create(&threadHandle, nullptr, entry, arg)
#define WaitThread(threadHandle) pthread_join(threadHandle, nullptr)

typedef void* (*ThreadEntry_t)(void*);

#else

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef HANDLE ThreadHandle_t;

#define MakeThread(threadHandle, entry, arg) threadHandle = CreateThread(nullptr, 0, entry, arg, 0, nullptr)
#define WaitThread(threadHandle) WaitForSingleObject(threadHandle, INFINITE)

typedef DWORD (*ThreadEntry_t)(LPVOID);

#endif

// TODO: Add worker support
class CThread
{
public:
    CThread() {}

    void Start(std::string name)
    {
        m_name = name;
        m_isTerminating = false;

        MakeThread(m_threadHandle, (ThreadEntry_t)ThreadProc, (void*)this);
        m_isRunning = true;
    }

    void Wait()
    {
        // Wait for termination
        WaitThread(m_threadHandle);
    }

    void StopThread(int* pReturnVal, bool wait = true)
    {
        if ( !m_isRunning )
            return;
        
        m_isTerminating = true;
        
        if ( wait )
        {
            Wait();
        }

        if ( pReturnVal )
        {
            *pReturnVal = m_returnVal;
        }
    }

    bool IsDone(int* pReturnCode)
    {
        if (!m_isRunning)
        {
            if ( pReturnCode )
                *pReturnCode = m_returnVal;
            return true;
        }

        return false;
    }
protected:
    virtual int Run() = 0;
private:
    static void ThreadProc(CThread* pThread)
    {
        pThread->m_returnVal = pThread->Run();

        pThread->m_isRunning = false;
        pThread->m_isTerminating = true;
    }

	CThread( const CThread& s ) {}
	void operator=( const CThread& s ) {}
private:
    std::string m_name;
    bool m_isRunning;
    volatile bool m_isTerminating;
    int m_returnVal;

    ThreadHandle_t m_threadHandle;
};