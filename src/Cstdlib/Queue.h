#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace CLib
{

template<typename T>
class Queue
{
public:
    void push(T item)
    {
        std::unique_lock<std::mutex> lock(m_Mutex);

        m_Queue.push(item);
        m_Cond.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(m_Mutex);

        while (m_Queue.size() == 0)
            m_Cond.wait(lock);

        T item = std::move(m_Queue.front());
        m_Queue.pop();

        return item;
    }
private:
    std::mutex m_Mutex;
    std::queue<T> m_Queue;
    std::condition_variable m_Cond;
};

}