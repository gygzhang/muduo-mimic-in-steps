#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "Channel.h"
#include"EventLoop.h"

class EventLoop;

class Acceptor
{
public:
    typedef std::function<void(int)> CallBack;
    Acceptor();
    Acceptor(int epollfd, int port);
    Acceptor(EventLoop *loop, int port);
    int listenOn(int port);
    void onConnection(int peerfd);
    void startListen(int port);
    void setCallBack(const CallBack cb){_connCallBack = cb;}
    ~Acceptor();
private:
    int _epollfd;
    int _listenfd;
    int _port;
    Channel *_pAcceptorChannel;
    CallBack _connCallBack;
    EventLoop *_loop;
};


#endif






