#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include"Channel.h"
#include<string>
class TcpConnection
{
public:
    typedef std::function<void()> CallBack; 
    TcpConnection();
    TcpConnection(int epollfd, int connfd);
    TcpConnection(int epollfd, int connfd, std::string localip, std::string peerip, int port);
    void onMessage();
    ~TcpConnection();

private:
    std::string _localip;
    std::string _peerip;
    int _port;
    int _epollfd;
    int _connfd;
    Channel* _pChannel;
};

#endif // !TCP_CONNECTION_H




