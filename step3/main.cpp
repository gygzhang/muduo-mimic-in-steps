#include"TcpServer.h"

int main(int argc, char **argv)
{
    TcpServer server;
    server.start(12345);
    return 0;
}