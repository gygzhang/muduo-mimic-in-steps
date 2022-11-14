#include"TcpServer.h"
#include<functional>

using namespace std;
using namespace std::placeholders;

TcpServer::TcpServer()
: _max_events(512), _max_listenfd(5), _port(12345)
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
    TcpConnectionPtr conn_ptr = new TcpConnection(_epollfd, connfd);
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
    _epollfd = epoll_create(1);
    _pAcceptor = new Acceptor(_epollfd, _port);
    _pAcceptor->setCallBack(std::bind(&TcpServer::onConnection, this, _1));
    _pAcceptor->startListen(port);
    
    for(;;)
    {
        cout<<"\n\nstaring polling..."<<endl;
        int num_events = epoll_wait(_epollfd, (epoll_event*)_events.data(), _max_events, -1);
        if(num_events<0) {
            cout<<"num_events<0"<<endl;
            break;
        }
        //cout<<"num_events: "<<num_events<<endl;
        
        for (int i = 0; i < num_events; i++)
        {
            ChannelPtr channel = static_cast<ChannelPtr>(_events[i].data.ptr);
            //ChannelPtr shared_channel = ChannelPtr(channel);
            // 
            _activeChannel.insert(std::make_pair(channel->getChannelFd(), channel));
        }

        for(std::map<int, ChannelPtr>::const_iterator it = _activeChannel.cbegin();
            it!=_activeChannel.cend();
            it++)
        {
            (*it).second->handleEvent();
        }
        
        // clear the active channel list, prevents from double-handling
        _activeChannel.clear();

    }
}


