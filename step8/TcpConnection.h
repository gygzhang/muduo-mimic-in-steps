#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include"Channel.h"
#include"EventLoop.h"
#include"CallBacks.h"
#include"Buffer.h"

#include<string>
#include<atomic>


class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    
    TcpConnection();
    TcpConnection(int epollfd, int connfd);
    TcpConnection(EventLoop *loop, int connfd);
    TcpConnection(std::string name, EventLoop *loop, int connfd);   
    TcpConnection(int epollfd, int connfd, std::string localip, std::string peerip, int port);
    void onMessage();
    

    //void handleConn();
    void handleRead();
    void handleWrite();

    void setConnCallBack(const ConnectionCallback &cb){_connCallBack = cb;}
    void setConnCloseCallBack(const ConnectionCloseCallback &cb){_connCloseCallback=cb;}
    void setMessageCallBack(const MessageCallback &cb){_messageCallBack = cb;}
    void setWriteCallBack(const CallBack &cb){_writeCallBack = cb;}

    void send(const char *msg);

    void connectionEstablished();
    

    std::string info(){return _peerIp+":"+std::to_string(_peerPort)+" -> "+_localIp+":"+std::to_string(_localPort)+"\n";}
    std::string getConnInfo(){return _peerIp+":"+std::to_string(_peerPort)+" -> "+_localIp+":"+std::to_string(_localPort);}
    int getConnFd(){return _connfd;}
    std::string getConnName(){return _name;}


    bool status(){return _status;}

    ~TcpConnection();
    

private:
    std::string _name; 
    std::string _localIp;
    std::string _peerIp;
    int _localPort;
    int _peerPort;
    int _epollfd;
    int _connfd;
    char *_buf;
    char *_wbuf;
    Buffer _iBuffer;
    Buffer _oBuffer;
    Channel* _pChannel;
    EventLoop *_loop;
    ConnectionCallback _connCallBack;
    ConnectionCloseCallback _connCloseCallback;
    MessageCallback _messageCallBack;
    CallBack _writeCallBack;
    std::atomic<bool> _status;
};

#endif // !TCP_CONNECTION_H




