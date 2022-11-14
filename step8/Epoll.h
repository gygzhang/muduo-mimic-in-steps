#ifndef EPOLL_H
#define EPOLL_H

#include<sys/socket.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<iostream>
#include<vector>
#include<set>
#include<map>
#include<memory>
#include<assert.h>
#include<unistd.h>

#include"Channel.h"
#include"EventLoop.h"

#include<vector>
#include<iostream>

#define BUFFER_SIZE 100
#define MAX_EVENTS 500
#define MAX_LISTENFD 5

class EventLoop;
class Channel;

class Epoll
{
public:
    typedef struct epoll_event EpollEvent;
    typedef Channel* ChannelPtr;
    typedef std::vector<Channel*> ChannelList;
    Epoll();
    Epoll(EventLoop* loop);
    ~Epoll();
    
    int poll(ChannelList& activeList);
    void addChannel(ChannelPtr channel);

private:
    EventLoop *_loop;
    void fillActiveChannel(ChannelList& activeList);
    int _epollfd;
    int _max_events;
    std::vector<EpollEvent> _events;
    //typedef std::vector<Channel*> ChannelList;
    ChannelList _channels;
};



#endif // !EPOLL_H
