#include "ThreadPool.h"
// #include "muduo/base/CountDownLatch.h"
#include "CurrentThread.h"
// #include "muduo/base/Logging.h"

#include <stdio.h>
#include <unistd.h>  // usleep
#include <iostream>

void print()
{
  printf("tid=%d\n", ::CurrentThread::getThreadID());
}

void printString(const std::string& str)
{
  std::cout << str;
  // print the str immediately
  std::cout.flush();
  usleep(100*1000);
}

void test(int maxSize)
{
  std::cout << "Test ThreadPool with max queue size = " << maxSize<<std::endl;
  ThreadPool pool("MainThreadPool");
  //pool.setMaxQueueSize(maxSize);
  pool.start(5);

  std::cout << "Adding";
  pool.run(print);
  pool.run(print);
  for (int i = 0; i < 100; ++i)
  {
    char buf[32];
    snprintf(buf, sizeof buf, ".");
    pool.run(std::bind(printString, std::string(buf)));
  }
  std::cout << "\nDone";

  //CountDownLatch latch(1);
  //pool.run(std::bind(&CountDownLatch::countDown, &latch));
  //latch.wait();
  pool.stop();
}

/*
 * Wish we could do this in the future.
void testMove()
{
  ThreadPool pool;
  pool.start(2);

  std::unique_ptr<int> x(new int(42));
  pool.run([y = std::move(x)]{ printf("%d: %d\n", CurrentThread::tid(), *y); });
  pool.stop();
}
*/

void longTask(int num)
{
  std::cout << "longTask " << num<<std::endl;
  CurrentThread::sleep(3000000);
}

void test2()
{
  std::cout << "Test ThreadPool by stoping early.";
  ThreadPool pool("ThreadPool");
  //pool.setMaxQueueSize(5);
  pool.start(3);

  Thread thread1([&pool]()
  {
    for (int i = 0; i < 20; ++i)
    {
      pool.run(std::bind(longTask, i));
    }
  }, "thread1");
  thread1.start();

  CurrentThread::sleep(5000000);
  std::cout << "stop pool";
  pool.stop();  // early stop

  thread1.join();
  // run() after stop()
  pool.run(print);
  std::cout << "test2 Done";
}

int ThreadPool_test()
{
  test(0);
  test(1);
  test(5);
  test(10);
  test(50);
  test2();
  return 0;
}
