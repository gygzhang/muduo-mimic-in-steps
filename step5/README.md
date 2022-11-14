###

本次新加了2个类：

1. EventLoop
2. Epoll

EventLoop类应该就是muduo中最重要的类了，muduo设计思想中的one loop per thread指的就是这个EventLoop类。这个类封装了epoll轮询io可用事件的循环。其内部持有Epoll类，Epoll类封装了epollfd，并提供一系列对epollfd进行操作的函数。

一条比较重要的调用路线：
当一个连接被Acceptor接收之后，Acceptor调用TcpServer提供的回调函数：构造一个TcpConnection类，并将这个类加入到TcpServer的连接列表之中。 TcpConnection类的构造函数会新建一个Channel类，并调用_pChannel->enableReading()，这个函数会调用Channel的update函数，Channel的update函数转而调用EventLoop类的update函数，EventLoop类的update函数转而调用EventLoop成员变量Epoll类的update函数，将TcpConnection类的fd加入epollfd之中。调用链如下：

TcpConnection::TcpConnection()->TcpConnection::_pChannel->enableReading() -> Channel::update() -> EventLoop::update() -> EventLoop::Epoll::update() -> epoll_ctl()
















