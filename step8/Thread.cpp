#include"Thread.h"
// #include"CurrentThread.h"

#include<unistd.h>
#include<sys/syscall.h>
#include<unistd.h>

#include<iostream>

// namespace CurrentThread
// {
//     __thread int _cachedThreadID = 0;
// }

void* start_routine(void *obj)
{
    //CurrentThread::_cachedThreadID = 0;
    static_cast<Thread*>(obj)->run();
    return (void*)0;
}

// Thread::Thread(Task task)
// :   _task(std::move(task)), 
//     _tid(0),
//     _ptid(0)
// {

// }

Thread::Thread(Task task, const std::string& name)
:   _task(std::move(task)), 
    _name(name),
    _tid(0),
    _ptid(0)
{
    
}

void Thread::run()
{
    _tid = CurrentThread::getThreadID();
    //std::cout<<&_task<<" running on tid: "<<_tid<<std::endl;
    _task();
}

void Thread::start()
{
    pthread_create(&_ptid, NULL, &start_routine, this);
}

int Thread::join()
{
    pthread_join(_ptid, NULL);
    return 0;
}

pid_t Thread::getThreadID()
{
    return static_cast<pid_t>(syscall(SYS_gettid));
}





