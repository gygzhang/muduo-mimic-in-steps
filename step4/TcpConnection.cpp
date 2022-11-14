#include"TcpConnection.h"

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

using namespace std;

TcpConnection::TcpConnection(int epollfd, int connfd)
    :_epollfd(epollfd), _connfd(connfd)
{
    _pChannel = new Channel(_epollfd, _connfd);
    _pChannel->setReadCallBack(std::bind(&TcpConnection::onMessage, this));
    _pChannel->enableReading();
    struct sockaddr_in addr;
    socklen_t socklen = sizeof(struct sockaddr_in);
    int rdfd = _connfd;
    //The getsockname function retrieves the local name for a socket.
    int res = getsockname(rdfd, (struct sockaddr*)&addr, &socklen);
    _localip = inet_ntoa(addr.sin_addr);
    res = getpeername(rdfd, (struct sockaddr*)&addr, &socklen);
    _peerip = inet_ntoa(addr.sin_addr);
    cout<<"new connection: "<<_peerip<<"->"<<_localip<<endl;
    //_pChannel->setReadCallBack(TcpConnection::);
}

void TcpConnection::onMessage(){
    struct sockaddr_in addr;
    socklen_t socklen = sizeof(struct sockaddr_in);
    int rdfd = _connfd;
    //The getsockname function retrieves the local name for a socket.
    int res = getpeername(rdfd, (struct sockaddr*)&addr, &socklen);
    assert(res>=0);
    if(rdfd<0){
        cout<<"rdfd<0: "<<errno<<endl;
        return;
    }
    char buf[BUFFER_SIZE];
    int nbytes=0;
    nbytes = read(rdfd, buf, BUFFER_SIZE);
    assert(nbytes>=0);
    if(nbytes<0){
        if(errno==ECONNRESET){
            cerr << "ECONNREST closed socket fd:" << rdfd << endl;
            close(rdfd);
        }
    }else if(nbytes==0){
        cerr<<"connection closed by peer. [ remote host: "<<inet_ntoa(addr.sin_addr)
            <<":"<<ntohs(addr.sin_port)<<"#"<<rdfd<<"]"<<endl;
        close(rdfd);
        //ChannelPtr pa = _channels.at(rdfd);
        //auto it = _channels.find(rdfd);
        //(*it).second.~unique_ptr();
        //_channels.erase(rdfd);
    }else{
        cout<<"recv from client: "<<buf<<endl;
        memset(buf,0,sizeof(buf));
        snprintf(buf, BUFFER_SIZE, "from server\n");
        if(write(rdfd, buf, nbytes)!=nbytes){
            cerr<<"write not finish at once. [ remote host: "<<inet_ntoa(addr.sin_addr)
            <<":"<<ntohs(addr.sin_port)<<"#"<<rdfd<<"]"<<endl;
        }
    }
}

TcpConnection::~TcpConnection()
{}

// TcpConnection::TcpConnection(int localip, int peerip, int port, int connfd)
    
// {

// }







