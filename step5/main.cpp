#include"TcpServer.h"

int main(int argc, char **argv)
{
    EventLoop eloop;
    TcpServer server(&eloop);
    server.start(12345);
    eloop.loop();
    return 0;
}