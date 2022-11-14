#include"TcpServer.h"
#include"EchoServer.h"
#include"TimeStamp.h"

#include <stdio.h>
#include <unistd.h>
#include<functional>

int cnt = 0;
EventLoop* g_loop;
// class TimeStamp;
void print(const char* msg)
{
    //printf("hi\n");
  printf("msg %s %s\n", TimeStamp::now().toString().c_str(), msg);
  if (++cnt == 20)
  {
    exit(-1);
    //g_loop->quit();
  }
}

int main(int argc, char **argv)
{
    // EventLoop eloop;
    // TcpServer server(&eloop);
    // server.start(12345);
    // eloop.loop();
    //EchoServer echo;
    //echo.start(12345);
    EventLoop loop;
    loop.runAfter(1, std::bind(print, "once31"));
    loop.runAfter(3, std::bind(print, "once32"));
    loop.runAfter(5, std::bind(print, "once10"));
    loop.runEvery(1.5, std::bind(print, "every 1.5"));
    loop.loop();
    return 0;
}