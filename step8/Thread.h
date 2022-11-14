#ifndef THREAD_H
#define THREAD_H

#include<functional>
#include<memory>
#include<string>
#include<pthread.h>
#include"CurrentThread.h"




class Thread
{
public:
    typedef std::function<void()> Task;
    //Thread()=default;
    // Thread(Task task);
    explicit Thread(Task task, const std::string& name=std::string());
    ~Thread()=default;
    void start();
    void run();
    int join();

    pid_t getThreadID();
    
private:
    pthread_t _ptid;
    pid_t _tid;
    Task _task;
    std::string _name;
};

#endif // !THREAD_H






