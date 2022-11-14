#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include "TcpServer.h"
#include "TcpConnection.h"


class EchoServer
{
public:
    typedef std::function<void()> CallBack;
    EchoServer();
    EchoServer(int port);
    void onConnection(const std::shared_ptr<TcpConnection>& conn);
    void onMessage(const std::shared_ptr<TcpConnection>& conn, const char* buf);
    ~EchoServer();

    void start(int port);
    void quit();

private:
    int _port;
    char *_buf;
    EventLoop *_loop;
    TcpServer *_serv;
    TcpConnection *_conn;

};

#endif





