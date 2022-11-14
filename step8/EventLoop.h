#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include<sys/epoll.h>

#include<iostream>
#include<atomic>
#include"Epoll.h"
#include"TimerQueue.h"
#include"TimeStamp.h"

#include"Mutex.h"

class Epoll;
class Channel;
class TimerQueue;

class EventLoop
{
public:
    typedef Channel* ChannelPtr;
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();
    void loop();
    void runInLoop(std::function<void()> f);
    void update(ChannelPtr channel);
    void quit();

    int creatWeakupFd();
    void handleRead();
    void wakeup();
    void addTaskInLoop(const Functor &task);
    void doPendingTasks();
    bool isInLoopThread();

    int64_t runAt(TimeStamp time, const TimerCallBack& cb);
    int64_t runAfter(double delay, const TimerCallBack& cb);
    int64_t runEvery(double interval, const TimerCallBack& cb);

private:
    //int _epollfd;
    std::atomic<bool> _quit;
    Epoll *_epoller;
    std::vector<ChannelPtr> _activeChannel;
    TimerQueue *_timerQueue;
    int _wakeupfd;
    std::unique_ptr<Channel> _wakeupChannel;
    std::vector<Functor> _pendingTasks;

    const pid_t _tid;

    bool _callingPendingFunctors;

};




#endif




