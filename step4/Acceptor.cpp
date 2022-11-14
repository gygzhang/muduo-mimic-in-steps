#include"Acceptor.h"
#include"Channel.h"
#include"TcpConnection.h"

#include<sys/socket.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<errno.h>

#include<iostream>

using namespace std;
using namespace std::placeholders;

Acceptor::Acceptor()
    : _epollfd(-1), _listenfd(-1)
{
    
}

Acceptor::Acceptor(int epollfd, int port)
    : _epollfd(epollfd), _listenfd(-1), _port(port), _connCallBack(nullptr)
{
    

}

void Acceptor::startListen(int port)
{
    _port = port;
    _listenfd = listenOn(port);
    _pAcceptorChannel = new Channel(_epollfd, _listenfd);
    _pAcceptorChannel->setReadCallBack(std::bind(&Acceptor::onConnection, this, _1));
    _pAcceptorChannel->enableReading();
    //cout<<"_listenfd: "<<_listenfd<<endl;
}

void Acceptor::onConnection(int peerfd1)
{
    struct sockaddr_in cli_addr1;
    socklen_t cli_len = sizeof(struct sockaddr_in);
    //cout<<peerfd1<<", "<<_listenfd<<endl;
    int peerfd = accept(_listenfd, (sockaddr*)&cli_addr1, &cli_len);
    if(peerfd>0){
        cout<<"new connection from "
            <<"["<<inet_ntoa(cli_addr1.sin_addr)
            <<":"<<ntohs(cli_addr1.sin_port)
            <<"#"<<peerfd<<"] accepted!"<<endl; 
    }else{
        cout<<"accept error: "<<errno<<endl;
    }

    fcntl(peerfd, F_SETFL, O_NONBLOCK);

    _connCallBack(peerfd);
    
    // TcpConnection *conn_ptr = new TcpConnection(_epollfd, peerfd);
    // Channel *channel = new Channel(_epollfd, peerfd);
    // //once channel readable, it will call onEvent
    // channel->setReadCallBack(std::bind(&TcpServer::onEvent, this, std::placeholders::_1));
    // channel->enableReading();
    // _channels.insert(std::make_pair(peerfd, channel));
}

// listen on port
int Acceptor::listenOn(int port)
{
    int on=1;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    /* allow immediate reuse of the port */
    fcntl(listenfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(on));
    servaddr.sin_family = AF_INET;
    in_addr idr;
    //inet_aton("0.0.0.0", &idr);
    //inet_pton
    //servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if(::bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr))<0){
        cout<<"bind: "<<errno<<endl;
    }
    if(listen(listenfd, MAX_LISTENFD)<0){
        cout<<"listen"<<endl;
    }
    cout<<"listen on port: "<<port<<endl;
    return listenfd;
}


Acceptor::~Acceptor()
{
    
}



