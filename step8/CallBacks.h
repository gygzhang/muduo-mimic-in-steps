#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <functional>
#include <memory>
#include"Buffer.h"

class TcpConnection;

typedef std::function<void()> CallBack;
typedef std::function<void()> TimerCallBack;
typedef std::function<void (const std::shared_ptr<TcpConnection>&)> ConnectionCallback;
typedef std::function<void (const std::shared_ptr<TcpConnection>&)> ConnectionCloseCallback;
typedef std::function<void (const std::shared_ptr<TcpConnection>&)> WriteCallback;
typedef std::function<void (const std::shared_ptr<TcpConnection>&, const char*)> MessageCallback;



#endif // !CALLBACKS_H






