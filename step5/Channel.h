#ifndef CHANNEL_H
#define CHANNEL_H

#include<functional>
#include<sys/epoll.h>

#include"EventLoop.h"

class EventLoop;

#define BUFFER_SIZE 100
#define MAX_EVENTS 500
#define MAX_LISTENFD 5

class Channel
{
public:
    typedef std::function<void(int fd)> EventCallBack;
    
    Channel(int epollfd, int channelfd);
    Channel(EventLoop *loop, int channelfd);
    ~Channel() = default;
    void setReadCallBack(const EventCallBack cb){ _readCallBack = cb; };
    void setWriteCallBack(const EventCallBack cb){ _writeCallBack = cb; };
    void setErrorCallBack(const EventCallBack cb){ _errorCallBack = cb; };
    void handleEvent();
    void setRevents(int revents){_revents = revents;};
    int getChannelFd() const {return _channelfd;};
    int getEvents(){return _events;}
    void enableReading() {_events |= EPOLLIN; update();};
private:
    void update();
    int _epollfd;
    int _channelfd;
    int _events;
    int _revents;
    EventCallBack _readCallBack;
    EventCallBack _writeCallBack;
    EventCallBack _errorCallBack;
    EventLoop *_loop;
};


#endif