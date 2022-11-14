#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include<functional>
#include<vector>

#include"BlockingQueue.h"
#include"Thread.h"
#include"Mutex.h"

class ThreadPool
{
public:
    typedef std::function<void()> Task;
    ThreadPool();
    ThreadPool(const std::string& name = "ThreadPool");
    ThreadPool(const char* name);
    void start(int numThreads);
    void addTask(const Task& task);
    void run();
    void run(const Task& task);
    void stop();

private:
    void runInThread();
    BlockingQueue<Task> _tasks;
    std::vector<Thread*> _threads;
    std::string _name;

    MutexLock _mutex;

};


#endif // !THREAD_POOL_H




