###

本次新加了两个类：
1. 一个是Acceptor类，这个类我认为也可以叫Listener类，负责在特定端口上监听将要到来的连接,其持有资源包括：
    - epoll fd
    - listen fd
    - listening port
    - channel of listen fd
    - callback

在监听端口的同时，Acceptor也应该向Channel注册自己的回调函数，告诉Channel：当epoll_wait()检测到关于我感兴趣的事件时， 你应该调用我提供的onConnection函数。 

TcpServer也向Acceptor类注册了一个回调函数，告诉Acceptor：当你那里有新的连接到来的时候，调用一下我提供的这个函数(我想把这些新来的连接加入到我的连接列表之中)。



2. 另一个是TcpConnection类，它负责接收Acceptor类接收的连接请求，将该连接抽象成一个对象，持有资源包括
    - local(server)ip
    - remote(client)ip
    - port
    - epoll fd
    - connection fd
    - channel of connection fd

前面说过，一个Channel类负责分发fd上的事件，TcpConnection代表这个连接已经建立了，因此我们目前最需要关注的事件是读取事件。因此我们需要向channel注册一个回调函数onMessage，告诉channel：当有关于我的消息时，调用我提供的这个函数。












