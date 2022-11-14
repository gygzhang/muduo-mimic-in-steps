#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include<sys/epoll.h>

#include<iostream>
#include<atomic>
#include"Epoll.h"

class Epoll;
class Channel;

class EventLoop
{
public:
    typedef Channel* ChannelPtr;
    EventLoop();
    ~EventLoop();
    void loop();
    void runInLoop(std::function<void()> f);
    void update(ChannelPtr channel);

private:
    //int _epollfd;
    std::atomic<bool> _quit;
    Epoll *_epoller;
    std::vector<ChannelPtr> _activeChannel;
};




#endif




