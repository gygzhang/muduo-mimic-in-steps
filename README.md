# muduo_8step

参考muduo，实现了一个非常简易的muduo

## step1： 熟悉epoll IO多路复用流程
原始socket编程+epoll

## step2： 增加TcpServer类
将step 1设计为一个TcpServer类

## step3： 增加Channel类
负责对一个socket fd进行管理

## step4:  增加Acceptor和TcpConnection类
Acceptor用于接收新的连接，TcpConnection类管理一个TCP连接

## step5:  增加Epoll和EventLoop类
EventLoop负责处理IO事件，主要功能是等待在epoll_wait上，直到有可用事件到来。Epoll类封装了epollfd，并提供一系列对epollfd进行操作的函数，以及提供活跃Channel给EventLoop使用。

## step6: 实现了一个经典的echo服务器
客户端连接到服务器，发送消息并向用户返回相同的信息

## step7: 实现定时器
使用timerfd实现了定时器

## step8：线程池
新增互斥锁，条件变量、阻塞队列、线程、线程池的实现
