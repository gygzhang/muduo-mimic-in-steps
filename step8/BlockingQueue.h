#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include<deque>
#include"Condition.h"
#include"Mutex.h"



template<typename T>
class BlockingQueue
{
public:
    BlockingQueue()
    :_cond(_mutex)
    {

    }

    ~BlockingQueue(){}

    void put(const T& obj)
    {
        MutexLockGuard lock(_mutex);
        _queue.push_back(obj);
        _cond.notify();
    }

    T take()
    {
        MutexLock lock(_mutex);
        while(_queue.empty())
        {
            _cond.wait();
        }
        T front(_queue.front());
        _queue.pop_front();
        return front;
    }

private:
    std::deque<T> _queue;
    MutexLock _mutex;
    Condition _cond;
    
};


#endif // !BLOCKINGQUEUE_H

