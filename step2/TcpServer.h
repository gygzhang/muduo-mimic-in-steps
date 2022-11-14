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

// using namespace std;

#define BUFFER_SIZE 100
#define MAX_EVENTS 500
#define MAX_LISTENFD 5

class TcpServer{
    typedef struct epoll_event EpollEvent;
    typedef std::unique_ptr<EpollEvent> EventPtr;
    public:
        TcpServer();
        TcpServer(int max_events, int max_listenfd);
        void listenOn(int port);
        void addEvent(EventPtr event_ptr);
        ~TcpServer()=default;
        void start(int port);
    private:
        int max_events_;
        int max_listenfd_;
        int listenfd_;
        int epollfd_;
        std::vector<EpollEvent> events_;
};


