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

#include<functional>

#include "Channel.h"
#include "TcpConnection.h"
#include "Acceptor.h"

#include "EventLoop.h"


// using namespace std;



class TcpServer{
    typedef struct epoll_event EpollEvent;
    typedef std::shared_ptr<EpollEvent> EventPtr;
    typedef Channel* ChannelPtr;
    typedef TcpConnection* TcpConnectionPtr;
    public:
        TcpServer();
        TcpServer(EventLoop *loop);
        TcpServer(int max_events, int max_listenfd, int port);
        void listenOn(int port);
        void addEvent(EventPtr event_ptr);
        ~TcpServer()=default;
        void start(int port);
        void onConnection(int connfd);
        void onMessage();
        void onEvent(int);
    private:
        int _max_events;
        int _max_listenfd;
        int _listenfd;
        int _epollfd;
        int _port;
        std::vector<EpollEvent> _events;
        std::unique_ptr<Channel> _pChannel;
        std::map<int, ChannelPtr> _channels;
        std::map<int, ChannelPtr> _activeChannel;
        std::map<int, TcpConnectionPtr> _conns;
        Acceptor *_pAcceptor;
        EventLoop *_loop;
        

        
};


