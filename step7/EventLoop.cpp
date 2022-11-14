#include"EventLoop.h"
using namespace std;
EventLoop::EventLoop()
:   _quit(false), 
    _timerQueue(new TimerQueue(this)),
    _epoller(new Epoll(this))
{
    
}

void EventLoop::loop()
{
    while(!_quit)
    {   
        //std::cout<<"_timerQueue size: "<<_timerQueue->getNumTimer()<<std::endl;
        int num_events = _epoller->poll(_activeChannel);
        (void)num_events;
        assert(num_events>=0);
        //std::cout<<"num_events: "<<num_events<<std::endl;
        for(auto it = _activeChannel.cbegin();
            it!=_activeChannel.cend();
            it++)
        {
            (*it)->handleEvent();
        }

        //exit(-1);
        
        // clear the active channel list, prevents from double-handling
        _activeChannel.clear();

    }
}

void EventLoop::runInLoop(std::function<void()> f)
{
    f();
    //cout<<"f"<<endl;
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


EventLoop::~EventLoop()
{

}






