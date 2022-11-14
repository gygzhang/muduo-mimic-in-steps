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
        _epoller->poll(_activeChannel);

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

void EventLoop::update(Channel* channel)
{
    _epoller->addChannel(channel);
}


EventLoop::~EventLoop()
{

}






