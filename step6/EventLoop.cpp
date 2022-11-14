#include"EventLoop.h"
using namespace std;
EventLoop::EventLoop()
: _quit(false)
{
    _epoller = new Epoll(this);
}

void EventLoop::loop()
{
    while(!_quit)
    {   
        int num_events = _epoller->poll(_activeChannel);
        //std::cout<<"num_events: "<<num_events<<std::endl;
        for(auto it = _activeChannel.cbegin();
            it!=_activeChannel.cend();
            it++)
        {
            (*it)->handleEvent();
        }
        
        // clear the active channel list, prevents from double-handling
        _activeChannel.clear();

    }
}

void EventLoop::runInLoop(std::function<void()> f)
{
    f();
    //cout<<"f"<<endl;
}

void EventLoop::update(Channel* channel)
{
    _epoller->addChannel(channel);
}


EventLoop::~EventLoop()
{

}






