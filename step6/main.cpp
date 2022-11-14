#include"TcpServer.h"
#include"EchoServer.h"
int main(int argc, char **argv)
{
    // EventLoop eloop;
    // TcpServer server(&eloop);
    // server.start(12345);
    // eloop.loop();
    EchoServer echo;
    echo.start(12345);
    return 0;
}