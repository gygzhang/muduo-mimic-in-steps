###

本次新加了2个类：

1. EchoServer

以及一个新的文件CallBacks.h

CallBacks.h 主要提供了几种回调函数的类型：
```c++
typedef std::function<void()> CallBack;
typedef std::function<void (const std::shared_ptr<TcpConnection>&)> ConnectionCallback;
typedef std::function<void (const std::shared_ptr<TcpConnection>&)> ConnectionCloseCallback;
typedef std::function<void (const std::shared_ptr<TcpConnection>&)> WriteCallback;
typedef std::function<void (const std::shared_ptr<TcpConnection>&, char*)> MessageCallback;
```
这些函数用于类之间注册自己的回调函数。

EchoServer实现了一个经典的echo服务器。


### 关于enable_share_from_this

muduo中的TcpConnection类使用了shared_ptr来包装，而在TcpServer类中又使用容器储存了shared_ptr<TcpConnection>, 这会产生两份shared_ptr<TcpConnection>，两份shared_ptr都不知道彼此的存在， 因此需要TcpConnection类继承enable_share_from_this（since c++11），从而让一个被shared_ptr管理的对象安全地被其他shared_ptr共享。enable_share_from_this这个类内部有一个weak_ptr,  当一个shared_ptr构造的时候，如果我们自定义的类继承了enable_share_from_this，那么就会初始化enable_share_from_this内部的weak_ptr，之后再在自定义类调用shared_from_this()时，就可以通过weak_ptr构造出shared_ptr。

要注意只能在shared_ptr对象上调用shared_from_this()，否则会出现UB（untill c++17）, 或者抛出std::bad_weak_ptr异常(since c++17)

[c++enable_shared_from_this源代码分析(from visutal studio 2017)_个人文章 - SegmentFault 思否](https://segmentfault.com/a/1190000020861953)



















