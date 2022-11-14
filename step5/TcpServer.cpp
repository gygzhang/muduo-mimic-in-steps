#include"TcpServer.h"
#include<functional>

using namespace std;
using namespace std::placeholders;

TcpServer::TcpServer()
: _max_events(512), _max_listenfd(5), _port(12345)
{
    _events.reserve(_max_events);
}

TcpServer::TcpServer(EventLoop *loop)
: _max_events(512), _max_listenfd(5), _port(12345), _loop(loop),
    _pAcceptor(new Acceptor(loop, _port))
{
    _events.reserve(_max_events);
}

TcpServer::TcpServer(int max_events, int max_listenfd, int port)
: _max_events(max_events), _max_listenfd(max_listenfd), _port(port)
{
    _events.reserve(_max_events);
}

void TcpServer::onConnection(int connfd)
{
    TcpConnectionPtr conn_ptr = new TcpConnection(_loop, connfd);
    _conns.insert(std::make_pair(connfd, conn_ptr));
    //or _conns[connfd] = conn_ptr;

}

void TcpServer::onEvent(int fd)
{
    cout<<"onEvent: "<<fd<<endl;
    if(fd==_listenfd){
        
        // ev.data.fd = fd;
        // ev.events = EPOLLIN;
        // if(epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev)<0){
        //     cerr<<"ctl error: "<<errno<<endl; 
        // }
    }else {
        
    }
}

void TcpServer::listenOn(int port)
{
    
}


void TcpServer::start(int port)
{
    //_epollfd = epoll_create(1);
    _pAcceptor = new Acceptor(_loop, _port);
    _pAcceptor->setCallBack(std::bind(&TcpServer::onConnection, this, _1));
    _pAcceptor->startListen(port);
}


