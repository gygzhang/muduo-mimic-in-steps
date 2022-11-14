#include "Thread.h"
#include "CurrentThread.h"
//#include "ThreadPool.h"
#include "thread_test/ThreadPool_test.h"
#include "thread_test/Thread_test.h"
#include"EventLoop.h"
#include "TcpServer.h"
#include "EchoServer.h"

#include <string>
#include <stdio.h>
#include <unistd.h>
#include <functional>

EventLoop *gev;

void f()
{
  printf("f tid=%d\n", ::CurrentThread::getThreadID());
}

void print1()
{
  printf(" print1 tid=%d\n", ::CurrentThread::getThreadID());
  //gev->runInLoop(std::bind(&f));
  gev->addTaskInLoop(std::bind(&f));
  //printf(" p1 tid=%d\n", ::CurrentThread::getThreadID());
}

void fun()
{
  sleep(3);
  printf("fun tid=%d\n", ::CurrentThread::getThreadID());
  gev->runInLoop(std::bind(&print1));
}


int main()
{
  // EventLoop eloop;
  // gev = &eloop;
  EchoServer echo;
  echo.start(12345);
  //Thread_test();
  //ThreadPool_test();
  // EventLoop ev;
  // gev = &ev;
  // Thread th1(std::bind(&fun));
  // Thread th2(std::bind(&fun));
  //th1.start();
  //CurrentThread::sleep(100000);
  //th2.start();
  //ev.runInLoop();
  //ev.runInLoop(std::bind(&fun));
  //ev.runInLoop(std::bind(&print));
  //ev.loop();
  

}
