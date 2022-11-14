### step 3

本次新加一个类:
1. Channel类，这个类负责对一个socket fd进行管理：
    - 设置感兴趣的事件(EPOLLIN等)
    - 管理用户对特定时间提供的回调函数
    - 对到来的特定事件进行分发(调用用户提供的各类回调函数)

```c++
void Channel::update()
{
    struct epoll_event ev;
    ev.data.ptr = this;
    ev.events = _events;
    //TODO: MOD should be considered
    epoll_ctl(_epollfd, EPOLL_CTL_ADD, _channelfd, &ev);
}
```
其中`ev.data.ptr = this`可能让人很困惑，这行代码要结合TcpServer.cpp中的这句代码来看：
```c++
ChannelPtr channel = static_cast<ChannelPtr>(_events[i].data.ptr);
```
因为epoll_wait会将发生的事件复制到一个struct epoll_event类型的数组中，通过设置`ev.data.ptr = this`，我们可以用_events[i].data.ptr来得到这个事件对应fd对应的channel，然后再调用channel的handleEvent函数来调用之前我们向channel中注册的各类函数(通过channel内的_events来确定具体的事件，然后调用对应的函数)。





