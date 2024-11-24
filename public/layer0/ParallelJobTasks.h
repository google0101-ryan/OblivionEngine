#pragma once

#include <vector>
#include <functional>
#include <layer0/Thread.h>

// Given a vector of stuff, call a user-specified function on a new thread
// With each element of the vector being passed to a single thread
// T is the typename of each element
template<typename T>
class CParallelListJob
{
public:
    CParallelListJob(std::vector<T>& work, std::function<void(T&)> func);

    void AddCompletionCallback(std::function<void(int)> callback)
    {
    }
};

template<typename T>
class CTaskThread : public CThread
{
public:
    void SetRunFunc(std::function<void(T&)> func)
    {
        this->func = func;
    }

    void SetParm(T& parm)
    {
        this->parm = parm;
    }

    virtual int Run()
    {
        func(parm);
        return 0;
    }
private:
    std::function<void(T&)> func = nullptr;
    T parm;
};

template <typename T>
inline CParallelListJob<T>::CParallelListJob(std::vector<T> &work, std::function<void(T &)> func)
{
    std::vector<CTaskThread<T>> threads(work.size());

    for (int i = 0; i < threads.size(); i++)
    {
        threads[i].SetParm(work[i]);
        threads[i].SetRunFunc(func);
        threads[i].Start("worker" + std::to_string(i));
    }

    // Wait for all threads to complete
    bool allDone = true;
    do
    {
        allDone = true;
        int dummy;
        for (auto& thread : threads)
        {
            if (!thread.IsDone(&dummy))
            {
                allDone = false;
                break;
            }
        }
    } while (!allDone);
}
