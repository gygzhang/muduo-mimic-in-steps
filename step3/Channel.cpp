#include "Channel.h"
#include<iostream>
using namespace std;
Channel::Channel(int epollfd, int channelfd)
    :_epollfd(epollfd)
    ,_channelfd(channelfd)
    ,_events(0) 
    ,_revents(0)
    ,_readCallBack(nullptr)
{
    
}

void Channel::update()
{
    struct epoll_event ev;
    ev.data.ptr = this;
    ev.events = _events;
    //TODO: MOD should be considered
    epoll_ctl(_epollfd, EPOLL_CTL_ADD, _channelfd, &ev);
}


void Channel::handleEvent()
{
    cout<<"handleEvent with fd: "<<_channelfd<<endl;
    if(_events&EPOLLIN){
        _readCallBack(_channelfd);
    }
}

