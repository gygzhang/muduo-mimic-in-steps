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

Channel::Channel(EventLoop *loop, int channelfd)
    :_epollfd(-1),
    _channelfd(channelfd),
    _events(0),
    _revents(0),
    _readCallBack(nullptr),
    _loop(loop)
{

}

void Channel::update()
{
    //cout<<"Channel::update(): "<<_loop<<endl;
    _loop->update(this);
}


void Channel::handleEvent()
{
    //cout<<"handleEvent with fd: "<<_channelfd<<endl;
    if(_events&EPOLLIN){
        //cout<<"handleEvent with fd: "<<_channelfd<<endl;
        _readCallBack(_channelfd);
        //cout<<"handleEvent with fd#2: "<<_channelfd<<endl;
    }
}

