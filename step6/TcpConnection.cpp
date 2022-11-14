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
using namespace std::placeholders;

TcpConnection::TcpConnection(int epollfd, int connfd)
    :_epollfd(epollfd), _connfd(connfd)
{
    _pChannel = new Channel(_epollfd, _connfd);
    //_pChannel->setConnCallBack(std::bind(&TcpConnection::handleConn, this));
    _pChannel->setReadCallBack(std::bind(&TcpConnection::handleRead, this));
    _pChannel->setWriteCallBack(std::bind(&TcpConnection::handleWrite, this));
    _pChannel->enableReading();
    struct sockaddr_in addr;
    socklen_t socklen = sizeof(struct sockaddr_in);
    int rdfd = _connfd;
    //The getsockname function retrieves the local name for a socket.
    int res = getsockname(rdfd, (struct sockaddr*)&addr, &socklen);
    _localIp = inet_ntoa(addr.sin_addr);
    res = getpeername(rdfd, (struct sockaddr*)&addr, &socklen);
    _peerIp = inet_ntoa(addr.sin_addr);
    cout<<"new connection: "<<_peerIp<<"->"<<_localIp<<endl;
    //_pChannel->setReadCallBack(TcpConnection::);
}

TcpConnection::TcpConnection(EventLoop *loop, int connfd)
    :_loop(loop), _connfd(connfd), _buf(new char[1024])
{
    _pChannel = new Channel(loop, _connfd);
    //_pChannel->setConnCallBack(std::bind(&TcpConnection::handleConn, this));
    _pChannel->setReadCallBack(std::bind(&TcpConnection::handleRead, this));
    //_pChannel->setWriteCallBack(std::bind(&TcpConnection::handleWrite, this));
    _pChannel->enableReading();
    struct sockaddr_in addr;
    socklen_t socklen = sizeof(struct sockaddr_in);
    int rdfd = _connfd;
    //The getsockname function retrieves the local name for a socket.
    int res = getsockname(rdfd, (struct sockaddr*)&addr, &socklen);
    _localIp = inet_ntoa(addr.sin_addr);
    _localPort = ntohs(addr.sin_port);
    res = getpeername(rdfd, (struct sockaddr*)&addr, &socklen);
    _peerIp = inet_ntoa(addr.sin_addr);
    _peerPort = ntohs(addr.sin_port);
    //cout<<"new connection: "<<_peerIp<<"->"<<_localIp<<endl;
    //_connCallBack(shared_from_this());
    //_pChannel->setReadCallBack(TcpConnection::);
}

TcpConnection::TcpConnection(string name, EventLoop *loop, int connfd)
    : TcpConnection(loop, connfd) 
{
    cout<<name<<endl;
    _name = name;
}

void TcpConnection::connectionEstablished()
{
    //cout<<"TcpConnection::connectionEstablished()"<<endl;
    _connCallBack(shared_from_this());
    //cout<<"TcpConnection::connectionEstablished()#2"<<endl;
}




void TcpConnection::handleRead()
{
    
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
    bzero(buf, BUFFER_SIZE);
    nbytes = read(rdfd, _buf, BUFFER_SIZE);
    
    assert(nbytes>=0);
    if(nbytes<0){
        if(errno==ECONNRESET){
            cerr << "ECONNREST closed socket fd:" << rdfd << endl;
            close(rdfd);
        }
    }else if(nbytes==0){
        cerr<<"connection closed by peer. [remote host: "<<inet_ntoa(addr.sin_addr)
            <<":"<<ntohs(addr.sin_port)<<"#"<<rdfd<<"]"<<endl;
        close(rdfd);
        _connCloseCallback(shared_from_this());
        //ChannelPtr pa = _channels.at(rdfd);
        //auto it = _channels.find(rdfd);
        //(*it).second.~unique_ptr();
        //_channels.erase(rdfd);
    }else{
        //cout<<"recv from client: "<<shared_from_this().get()<<endl;
        //
        //https://stackoverflow.com/questions/27697973/shared-from-this-causing-bad-weak-ptr
        //https://stackoverflow.com/questions/712279/what-is-the-usefulness-of-enable-shared-from-this
        
        assert(_messageCallBack!=nullptr);
        _messageCallBack(shared_from_this(), _buf);
        //cout<<"haha"<<endl;
        //return;  
    }
    //cout<<"TcpConnection::handleRead()#2"<<endl;
}

void TcpConnection::handleWrite()
{
    int nbytes = sizeof(_wbuf);
    memset(_wbuf,0,sizeof(_wbuf));
    string backMesg;
    backMesg = "from server: "+ _localIp+ " to peer " + _peerIp+"\n";
    snprintf(_wbuf, BUFFER_SIZE, backMesg.c_str());
    if(write(_connfd, _wbuf, nbytes)!=nbytes){
        cerr<<"write not finish at once. [ remote host: "<<_peerIp
        <<":"<<_localPort<<"#"<<_connfd<<"]"<<endl;
    }
}



void TcpConnection::onMessage()
{
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
    //cout<<"TcpConnection::onMessage()"<<endl;
    char buf[BUFFER_SIZE];
    int nbytes=0;
    bzero(buf, BUFFER_SIZE);
    nbytes = read(rdfd, buf, BUFFER_SIZE);
    assert(nbytes>=0);
    if(nbytes<0){
        if(errno==ECONNRESET){
            cerr << "ECONNREST closed socket fd:" << rdfd << endl;
            close(rdfd);
        }
    }else if(nbytes==0){
        cerr<<"connection closed by peer. [remote host: "<<inet_ntoa(addr.sin_addr)
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
    //cout<<"TcpConnection::onMessage()#2"<<endl;
}

void TcpConnection::send(const char *msg)
{
    //memset(buf,0,sizeof(buf));
    //snprintf(buf, BUFFER_SIZE, "from server\n");
    //cout<<"TcpConnection::send: "<<msg<<endl;
    if(write(_connfd, msg, strlen(msg))!=strlen(msg)){
        cerr<<"write not finish at once. [ remote host: "<<_peerIp
        <<":"<<_localPort<<"#"<<_connfd<<"]"<<endl;
    }
}



TcpConnection::~TcpConnection()
{
    delete []_buf;
    //_connCallBack(shared_from_this());
    //_connCloseCallback(shared_from_this());
    //cout<<"TcpConnection::~TcpConnection"<<endl;
}

// TcpConnection::TcpConnection(int localip, int peerip, int port, int connfd)
    
// {

// }







