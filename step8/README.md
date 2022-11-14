## step8

### 新增类
本次新加了x个类：

1. MutexLock
2. MutexLockGuard
3. Condition
4. BlockingQueue
4. Thread
1. CurrentThread
5. ThreadPool
---

### 类的解释

#### MutexLock
其中， MutexLock类封装了pthread的mutex：`pthread_mutex_t`，该类提供了对互斥量的基本操作：初始化，上锁，解锁，销毁。

#### MutexLockGuard
MutexLockGuard实现了RAII的思想，即资源获取即是初始化，在作用域内，初始化一个MutexLockGuard对象的同时就会对MutexLock的mutex上锁，当离开作用域时，就会对mutex进行解锁操作。这是通过类的构造函数和析构函数实现的。

#### Condition
Condition类对pthread的condition变量`pthread_cond_t`进行了封装。 条件变量可以实现多线程之间的同步，在本项目中，条件变量用来同步多线程对任务队列的访问：
- 当需要放入一个任务在队列中时，我们使用MutexLockGuard来获取互斥锁，获取到了就将任务放入队列之中，如果队列正在被其他任务访问，那么后来的线程将陷入睡眠状态，直到正在访问队列的线程释放互斥锁(发生在MutexLockGuard销毁的时候)。
- 在放入之后，会调用condition varible的notify函数来唤醒等待在该条件变量上的线程(wait发生在队列为空且有线程尝试从该空队列获取任务时)。

#### BlockingQueue

阻塞队列实现了在多线程环境下对一个队列的各种操作，包括：放入任务，取出任务等基本功能。

#### Thread & CurrentThread

Thread类封装了pthread，包括其pthread_t，线程id等资源：

- `pthread_t` 用于对该pthread线程进行一系列操作
- `pid_t` 线程的PID，为下面的第二个ID：
  - `getpid()`得到的是进程的PID
  - 要得到线程的PID，需要使用系统调用：`syscall(SYS_gettid)`, 这个ID在是唯一的，在内核空间中。
  - pthread有一个函数`pthread_self()`，这个函数获取的是进程中的线程ID，不同进程的ID可能是一样的。

以及对这些资源的一系列操作：

- `pthread_start()`: 调用`pthread_create(&_ptid, NULL, &start_routine, this)` 创建一个新的pthread线程, 其参数start_routine为一个自定义函数，this是传递给start_routine的参数，指定当线程创建时应该执行的函数。其内容如下:
    ```c++
    void* start_routine(void *obj)
    {
        //CurrentThread::_cachedThreadID = 0;
        static_cast<Thread*>(obj)->run();
        return (void*)0;
    }
    //run函数的定义如下
    void Thread::run()
    {
        _tid = CurrentThread::getThreadID();
        _task();
    }
    ```
可以看到，我们其实就是让pthread线程创建之后就执行其内部的task(Thread创建时用std::bind传递的std::function类型的参数)。

- join()函数，用于回收线程。

CurrentThread一个命名空间，用于在任意地方获取当前线程的相关信息，比如pid、name等。其实现用到了`__thread`关键字, 这个关键字，用于创建不同线程的全局变量，如果没有`__thread`修饰，那么一个全局变量在程序的运行空间的只有一份，但是如果被`__thread`修饰，那么每个线程都会有一个全局变量，他们的值可以不同。 

CurrentThread.h里面使用inline修饰了一个变量:
```c++
inline __thread int _cachedThreadID;
```
这是因为在头文件中定义变量，会使得编译的时候报`multiple definition of CurrentThread::_cachedThreadID`，这个错是因为头文件被多个编译单元include了。

inline是C++ 17的新的特性，看网上说是为了支持header only library。用这个就可以避免同一个header被多个源文件包含报的错。

还可以使用extern来达到相同的功能，在header中extern一个变量，在source文件中来定义它，之后再其他source文件中都可以使用这个变量了，不必在每个source文件中都使用extern来引用这个变量。

关于header only library:
总结一下，就是由于C++包管理工具的限制，header only library的好处就是不用担心工具链的冲突，缺点是编译时间会很久。

参考：

[C++ 类定义 header only 是一种值得推崇的做法吗？ - 知乎](https://www.zhihu.com/question/35461702)


[p-ranav/awesome-hpp: A curated list of awesome header-only C++ libraries](https://github.com/p-ranav/awesome-hpp)

[c++ - Benefits of header-only libraries - Stack Overflow](https://stackoverflow.com/questions/12671383/benefits-of-header-only-libraries)


---

#### ThreadPool

ThreadPool实现了一个非常简易的线程池，使用可以创建一个ThreadPool类，然后调用ThreadPool::start(nthreads)来运行线程池，nthreads指定了创建pthread的数量，然后通过调用ThreadPool::run(task)在线程池中运行任务。这个线程池非常简易，打算后面升级一下。


#### EventLoop的中的唤醒

在io线程等待io到来的时候，有时候我们可能希望它去干一些其他事情，比如一些简单的计算，因此我们使用linux的eventfd相关函数来实现这个功能。

eventfd是一个文件描述符，事件通知都是基于对这个文件描述符的操作。我们创建一个eventfd，并将其读事件注册到EventLoop， 大部分时间，EventLoop应该都等待在`epoll_wait`上的，当我们需要唤醒EventLoop时，我们就可以通过向eventfd进行写操作，从而出发eventfd的可读事件，从而让`epoll_wait`返回，以便来处理pendingTasks。

需要注意，我们并没有在`handleRead`中直接执行`doPendingTasks`,这是因为因为如果在`handleRead`中直接执行`doPendingTasks`, 考虑下面两种情况：
- 某个pendingTask又恰好调用`addTaskInLoop`添加的新的pendingTask2
- 或者和eventfd同时到达的fd所在的fd的回调函数调用了`addTaskInLoop`

在这两种情况中，我们都需要调用wakeup函数使下一次epoll_wait函数返回，从而尽快执行我们的pendingTask2，不然我们刚刚添加的的pendingTask2下一次执行时间就只能是下一次epoll_wait返回的时候，这个时间是不确定的。但是缺点也很明显，我们增加了三次系统调用，`write`-`epoll_wait`-`read`。这个开销完全可以避免：等channel的回调处理完，统一处理pendingTasks。因此需要将`doPendingTasks`放到`loop`的最后。



### mutex和condition的异同

- mutex是用来为一个共享资源提供互斥访问
- condition是用来为等待一个条件(标志位)变为true

下面这种使用mutex实现condition的方式是错的:
```c++
lock(mutex)
while (!flag) {
    sleep(100);
}
unlock(mutex)

do_something_on_flag_set();
```

因为在sleep中，本线程会一直持有mutex，其他线程是没有机会修改flag的（这个flag受同一个mutex保护）。可以使用std::atomic<bool>来实现一个类似的条件变量，但是有一个问题，假设我们要实现一个生产者消费者模型：
- 如果只有很一个消费者，当队列中没有数据可消费时，消费者就会一直轮询检测队列，这会一直消费CPU的cycle，直到数据到来，因为消费者很少，问题不大。
- 但是如果是很多个消费者，浪费的CPU cycle就不能被忽略了。

没有条件变量，轮询会非常浪费CPU资源，线程会busy waiting。而条件变量是一种不需要轮询就可以实现相同功能的方法。

条件变量总是和互斥锁一起使用，因为条件变量属于贡献资源，多个线程使用同一个条件变量，会引发竞态(race condition)。因此需要互斥锁来使得对条件变量的访问可以互斥进行。


### yield和sleep

- yield会将线程重新插入到操作系统的调度队列中
- sleep会阻塞当前线程， 将该线程放入睡眠线程队列，只有请求睡眠的时候过去，调度器才会将线程重新插入调度队列。sleep(0)可能会实现为yield。

因此，如果在如果系统中，一个线程在yield上循环，此时系统中又没有其他的任务，那么运行该线程的CPU利用率马上会提高到100%，因为没有其他线程需要执行，这个线程马上就会从待调度队列中取出继续运行。而是用sleep相关函数，该线程会被放到睡眠队列，可以预料到CPU利用率不会变高，但是要注意一种情况，当睡眠的时间很短的时候，CPU负载会立即变高，因为CPU会将sleep的线程反复在睡眠队列和调度队列中移动。

### 自旋锁和互斥锁

- 自旋锁
- 互斥锁

当一个线程尝试lock一个mutex没有成功时(因为互斥锁已经被其他线程获取了)，这个时候这个线程将sleep，从而运行其他线程执行，而它自己则将一直休眠到有被操作系统唤醒，被操作系统唤醒发生在当mutex被前一个holder解锁。

当线程获取一个自旋锁时，如果没有成功，它会不断重试加锁操作，知道最终成功。因此它不会放弃对CPU的控制权(当他的时间片消耗完，操作系统还是会将其切换到另一个线程)。

互斥锁的问题在于，让线程进入睡眠并再次唤醒它们都是非常昂贵的操作，这会消耗非常多的CPU cycle。 因此如果持有锁的时间非常短，那么让线程进入睡眠并再次唤醒它们的时间完全有可能超过它们实际睡眠的时间，这样显然不如自旋锁：不放弃CPU，因此也就不会产生上下文切换，自然节省了很多的CPU cycle。

>比较短的简单代码可以应该使用自旋锁,当锁持有时间很长的时候，应该使用互斥锁

参考：
- https://stackoverflow.com/questions/5869825/when-should-one-use-a-spinlock-instead-of-mutex
- https://unix.stackexchange.com/questions/5107/why-are-spin-locks-good-choices-in-linux-kernel-design-instead-of-something-more

- https://stackoverflow.com/questions/17325888/c11-thread-waiting-behaviour-stdthis-threadyield-vs-stdthis-thread



















