###

本次新加了3个类：

1. TimeStamp
2. Timer
3. TimerQueue

TimeStamp类封装了从epoch(1970-01-01 00:00:00 +0000 (UTC))到现在过去的微秒数(us), 提供给Timer使用，比如要一个Timer在2秒之后运行，那么应该提供给定时器一个TimeStamp对象，它内部的时间应该是设置定时器时epoch到现在的微秒数加上2*100*100us。

Timer类封装了定时器调用时间点(TimeStamp)、定时器回调函数以及周期定时器所需要的一系列状态。每个定时器都有一个unique的编号，为了避免多线程数据竞争，所以使用了原子变量。

TimerQueue将Timer和EventLoop联系了起来，TimerQueue使用EventLoop指针来初始化自己， EventLoop内部有一个TimerQueue对象。 TimerQueue内部有一个关于[timerfd](https://man7.org/linux/man-pages/man2/timerfd_create.2.html)的Channel类, 把timerfd加入epoll轮询队列中，当定时器过期的时候，epoll_wait就会返回该定时器过期(expiration)的时间，之后就可以调用Timer类的回调函数，执行目标函数。

### 定时器超时到目标函数执行，流程是什么？

首先，定时器的设置是通过EventLoop实现的，EventLoop类根据参数(时间、目标函数)将其对应的定时器(Timer类)加入到的自己的定时器队列中(TimerQueue类)。 TimerQueue类会调用linux系统调用`timerfd_settime`设置它自己的timerfd的超时时间，需要注意设置的是绝对时间，比如要一个函数2s后执行，那么应该通过`timerfd_settime`设置timerfd的超时时间为2s, 而不是从epoch到现在的时间。timerfd对应的Channel是通过EventLoop和timerfd构建的，EventLoop内部的Epoll类会监听这个timerfd，当超时之后，Channel会调用TimerQueue类提供的回调函数，将程序控制权转回到TimerQueue类，TimerQueue类会去查找所有过期的定时器，并分别执行这些定时器对应的目标函数，之后再重置这些定时器--不是周期定时器就删除，否则就重新加入待执行定时器列表。

### 只有一个timerfd，多个定时器如何正确地被执行的？

当前一个定时器A加入到TimerQueue类，又加入一个定时器B，timerfd的超时时间会被设置为B的调用时间，如果B的调用时间在A后面，那么不会去改变tiemrfd的超时时间，而是当A超时之后，将timerfd的超时时间设置为下一个调用时间最早的定时器。如果B的调用时间在A前面，那么就会通过修改timerfd的超时时间，来保证A能在预期的时间点调用。

### 使用std::set中的一个问题
在调用`_sortedTimers.begin()->first.toString()`会报` error: passing ‘const TimeStamp’ as ‘this’ argument discards qualifiers`这样的错误，_sortedTimers是一个std::set,内部储存的是std::pair<TimeStamp, Timer*>，TimeStamp的toString()方法为非const方法， 错误的原因在于std::set内部储存的TimeStamp是const TimeStamp, C++里面不能在一个const对象上调用非const方法， 因此应该将toString()声明为const方法，问题得到解决。


### 获取超时定时器函数中，哨兵的作用是什么？

在TimerQueue中使用getExpiredTimer函数获取超时的定时器，里面首先定义了一个哨兵Entry：
```c++
Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
```
这句代码使用当前时间`now`和`UINTPTR_MAX`来定义一个哨兵，之后调用:
```c++
TimerList::iterator end = _sortedTimers.lower_bound(sentry);
```
来获取所有超时的定时器。

需要注意，TimerQueue使用一个std::set来储存定时器，它会有序的，在插入新的元素的时候，会以升序来放置新的元素，std::set存放的还是一个std::pair, 它的比较函数逻辑是: 先比较第一个元素，一样就比较第二个元素，以此类推。

lower_bound会获得第一个不满足`item < sentry`的`item`, 就是说，会返回指向第一个没有过期的定时器的迭代器，在它之前的所有的定时器都是已经超时了的。

UINTPTR_MAX作用是，如果now刚好和定时器超时时间一样。 如果我们只看`item.callTime`, 由于lower_bound返回的是第一个大于或者等于的元素，那么lower_bound就会返回第一个超时时间和now一样的定时器(因为`item.callTime`==`now.callTime`，满足lower_bound的返回条件)，因此这些超时时间和now一样的定时器不会被加入到待执行列表中，所以也就就得不到执行，或者只能等下一次定时器超时事件发生，因此我们应该返回下一个超时时间点的定时器，使用UINTPTR_MAX可以让lower_bound对超时时间和now一样的定时器和sentry比较结果不满足lower_bound的返回条件而被跳过，从而指向下一个超时的时间点的定时器(下一个超时的时间点的定时器的超时时间大于now这个时间点， 满足lower_bound的返回条件，从而返回)。





















