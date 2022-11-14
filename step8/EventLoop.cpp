#include"EventLoop.h"
#include"CurrentThread.h"
#include<sys/eventfd.h>

using namespace std;
EventLoop::EventLoop()
:   _quit(false), 
    _timerQueue(new TimerQueue(this)),
    _epoller(new Epoll(this)),
    _wakeupfd(creatWeakupFd()),
    _callingPendingFunctors(false),
    _wakeupChannel(new Channel(this, _wakeupfd)),
    _tid(CurrentThread::getThreadID())
{
    _wakeupChannel->setReadCallBack(std::bind(&EventLoop::handleRead, this));
    _wakeupChannel->enableReading();
}

void EventLoop::loop()
{
    while(!_quit)
    {   
        int num_events = _epoller->poll(_activeChannel);
        (void)num_events;
        assert(num_events>=0);
        for(auto it = _activeChannel.cbegin();
            it!=_activeChannel.cend();
            it++)
        {
            (*it)->handleEvent();
        }

        // handling pending task here to avoid 3 system call
        doPendingTasks();
        
        // clear the active channel list, prevents from double-handling
        _activeChannel.clear();

        

    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = ::read(_wakeupfd, &one, sizeof(one));
    assert(n==sizeof(one));
    doPendingTasks();
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = ::write(_wakeupfd, &one, sizeof(one));
    assert(n==sizeof(one));
}

void EventLoop::addTaskInLoop(const Functor &task)
{
    {
        MutexLockGuard lock(_mutex);
        _pendingTasks.push_back(std::move(task));
    }
    
    if(!isInLoopThread()||_callingPendingFunctors)
    {
        wakeup();
    }
}

void EventLoop::doPendingTasks()
{
    std::vector<Functor> tasks;
    _callingPendingFunctors = true;
    {
        MutexLockGuard lock(_mutex);
        tasks.swap(_pendingTasks);
    }
    for(const Functor& task : tasks)
    {
        task();
    }
    _callingPendingFunctors = false;
}

void EventLoop::runInLoop(std::function<void()> f)
{
    if(isInLoopThread())
    {
        f(); 
    }
    else
    {
        addTaskInLoop(std::move(f));
    }
}

void EventLoop::quit()
{
    _quit = true;
}

int64_t EventLoop::runAt(TimeStamp time, const TimerCallBack& cb)
{
    return _timerQueue->addTimer(std::move(cb), time, 0);
}

int64_t EventLoop::runAfter(double delay, const TimerCallBack& cb)
{
    TimeStamp time(TimeStamp::addTime(TimeStamp::now(), delay));
    return runAt(time, cb);
}

int64_t EventLoop::runEvery(double interval, const TimerCallBack& cb)
{
    TimeStamp time(TimeStamp::addTime(TimeStamp::now(), interval));
    return _timerQueue->addTimer(std::move(cb), time, interval);
}

void EventLoop::update(Channel* channel)
{
    //cout<<"EventLoop::update: "<<channel<<endl;
    _epoller->addChannel(channel);
}

int EventLoop::creatWeakupFd()
{
    int eventfd = ::eventfd(0, EFD_NONBLOCK| EFD_CLOEXEC);
    assert(eventfd>=0);
    return eventfd;
}


EventLoop::~EventLoop()
{
    
}

bool EventLoop::isInLoopThread()
{
    return _tid == CurrentThread::getThreadID();
}






