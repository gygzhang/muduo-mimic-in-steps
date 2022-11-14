#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <functional>
#include <memory>

class TcpConnection;

typedef std::function<void()> CallBack;
typedef std::function<void (const std::shared_ptr<TcpConnection>&)> ConnectionCallback;
typedef std::function<void (const std::shared_ptr<TcpConnection>&)> ConnectionCloseCallback;
typedef std::function<void (const std::shared_ptr<TcpConnection>&)> WriteCallback;
typedef std::function<void (const std::shared_ptr<TcpConnection>&, char*)> MessageCallback;



#endif // !CALLBACKS_H






