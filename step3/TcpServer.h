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


// using namespace std;

#define BUFFER_SIZE 100
#define MAX_EVENTS 500
#define MAX_LISTENFD 5

class TcpServer{
    typedef struct epoll_event EpollEvent;
    typedef std::shared_ptr<EpollEvent> EventPtr;
    typedef Channel* ChannelPtr;
    public:
        TcpServer();
        TcpServer(int max_events, int max_listenfd);
        void listenOn(int port);
        void addEvent(EventPtr event_ptr);
        ~TcpServer()=default;
        void start(int port);
        void onConnection();
        void onMessage();
        void onEvent(int);
    private:
        int _max_events;
        int _max_listenfd;
        int _listenfd;
        int _epollfd;
        std::vector<EpollEvent> _events;
        std::unique_ptr<Channel> _pChannel;
        std::map<int, ChannelPtr> _channels;
        std::map<int, ChannelPtr> _activeChannel;
        
};


