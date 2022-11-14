#include"EchoServer.h"

using namespace std;
using namespace std::placeholders;

EchoServer::EchoServer()
:   _loop(new EventLoop()),
    _serv(new TcpServer(_loop)),
    _buf(new char[1024])
{
    _serv->setConnCallBack(std::bind(&EchoServer::onConnection, this, _1));
    _serv->setMessageCallBack(std::bind(&EchoServer::onMessage, this, _1, _2));
}

EchoServer::EchoServer(int port)
:   _loop(new EventLoop()),
    _serv(new TcpServer(_loop)),
    _buf(new char[1024]),
    _port(port)
{
    //_port = port;
    _serv->setConnCallBack(std::bind(&EchoServer::onConnection, this, _1));
    _serv->setMessageCallBack(std::bind(&EchoServer::onMessage, this, _1, _2));
}

void EchoServer::start(int port)
{
    _serv->start(port);
    _loop->loop();
}

void EchoServer::onConnection(const std::shared_ptr<TcpConnection>& conn)
{
    //cout<<"EchoServer::onConnection: "<<conn->info();
    _serv->setConnCallBack(std::bind(&EchoServer::onConnection, this, _1));
    _serv->setMessageCallBack(std::bind(&EchoServer::onMessage, this, _1, _2));
}

void EchoServer::onMessage(const std::shared_ptr<TcpConnection>& conn, const char* buf)
{
    cout<<"EchoServer::onMessage: send "<<buf<<endl;
    string backMesg;
    backMesg = "from server: "+ conn->getConnInfo()+" ^^^^ "+buf+"\n";
    //bzero(buf,1024);
    //snprintf(buf, BUFFER_SIZE, backMesg.c_str());
    //cout<<"buf: "<<backMesg<<endl;
    conn->send(backMesg.c_str());
}

EchoServer::~EchoServer()
{
    
}



