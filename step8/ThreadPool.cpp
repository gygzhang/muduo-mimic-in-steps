#include"ThreadPool.h"

ThreadPool::ThreadPool()
{

}

ThreadPool::ThreadPool(const std::string& name)
:_name(name)
{

}

ThreadPool::ThreadPool(const char* name)
:_name(name)
{

}

void ThreadPool::start(int nthreads)
{
    _threads.reserve(nthreads);
    int i=0;
    std::for_each(_threads.begin(), _threads.end(), [&](auto it){
        Thread *pth = new Thread(std::bind(&ThreadPool::runInThread, this), _name + std::to_string(i++) );
        _threads.push_back(pth);
        pth->start();
    } 
    );
}


void ThreadPool::addTask(const Task& task)
{
    _tasks.put(task);
}

void ThreadPool::run()
{
    runInThread();
}

void ThreadPool::run(const Task& task)
{
    task();
}

void ThreadPool::runInThread()
{
    while(true)
    {
        _tasks.take()();
    }
}

void ThreadPool::stop()
{
    for(auto th : _threads)
    {
        th->join();
    }
}


