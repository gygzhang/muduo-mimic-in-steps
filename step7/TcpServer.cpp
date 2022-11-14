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
    std::shared_ptr<TcpConnection> pconn = std::make_shared<TcpConnection>(string("conn "+std::to_string(connfd)), _loop, connfd);
    //cout<<"TcpServer::onConnection: "<<pconn<<endl;
    pconn->setConnCallBack(_connCallBack);
    pconn->setConnCloseCallBack(std::bind(&TcpServer::removeConnection, this, _1));
    assert(_messageCallBack!=nullptr);
    pconn->setMessageCallBack(_messageCallBack);
    pconn->setWriteCallBack(_writeCallBack);
    assert(pconn!=nullptr);
    _loop->runInLoop(std::bind(&TcpConnection::connectionEstablished, pconn));
    // https://stackoverflow.com/questions/17172080/insert-vs-emplace-vs-operator-in-c-map
    // bug: Because std::map does not allow for duplicates if there is 
    // an existing element it will not insert anything.

    /*** 
     reason of segement fault: Because pconn is a shared pointer, so if it not       
     been inserted into a container, the dtor of TcpConnection will be called 
     as soon as this function ends,  as a result, _messageCallBack will be called 
     in the future with a destroyed object, segement fault occur!
    ***/

    //_conns.insert(std::make_pair(connfd, pconn));
    _conns[connfd] = pconn;
    printActiveConnections();
}

void TcpServer::removeConnection(const std::shared_ptr<TcpConnection> conn)
{
    _conns.erase(conn->getConnFd());
}

void TcpServer::printActiveConnections()
{
    cout<<"\nconnName           info             adress"<<endl;
    for(auto it: _conns){
        cout<<it.second->getConnName()<<"     "<<it.second->getConnInfo()<< "    "<<it.second.get()<<endl;
    }
    cout<<endl;
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


