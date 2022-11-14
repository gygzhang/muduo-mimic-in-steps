#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include<sys/epoll.h>

#include<iostream>
#include<atomic>
#include"Epoll.h"
#include"TimerQueue.h"
#include"TimeStamp.h"

class Epoll;
class Channel;
class TimerQueue;

class EventLoop
{
public:
    typedef Channel* ChannelPtr;
    EventLoop();
    ~EventLoop();
    void loop();
    void runInLoop(std::function<void()> f);
    void update(ChannelPtr channel);

    int64_t runAt(TimeStamp time, const TimerCallBack& cb);
    int64_t runAfter(double delay, const TimerCallBack& cb);
    int64_t runEvery(double interval, const TimerCallBack& cb);

private:
    //int _epollfd;
    std::atomic<bool> _quit;
    Epoll *_epoller;
    std::vector<ChannelPtr> _activeChannel;
    TimerQueue *_timerQueue;
};




#endif




