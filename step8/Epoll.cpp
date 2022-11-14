#include"Epoll.h"

using namespace std;
Epoll::Epoll()
{

}

Epoll::Epoll(EventLoop* loop)
:   _loop(loop),
    _max_events(512),
    _epollfd(::epoll_create(1))
{
    _events.reserve(_max_events);
}

void Epoll::addChannel(ChannelPtr channel){
    int fd = channel->getChannelFd();
    struct epoll_event ev;
    ev.data.ptr = channel;
    ev.events = channel->getEvents();
    ev.events |= EPOLLIN;
    //TODO: MOD should be considered
    //cout<<"add fd: "<<_epollfd<<", "<<fd<<endl;
    epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev);
}

int Epoll::poll(ChannelList& activeList)
{
    //cout<<"polling on "<<TimeStamp::now().toString()<<endl;
    int num_events = epoll_wait(_epollfd, (epoll_event*)_events.data(), _max_events, -1);
    if(num_events<0) {
        cout<<"num_events<0: "<<errno<<endl;
        //exit(-1);
        return -1;
    }
    //cout<<"num_events: "<<num_events<<endl;
    
    for (int i = 0; i < num_events; i++)
    {
        ChannelPtr channel = static_cast<ChannelPtr>(_events[i].data.ptr);
        //ChannelPtr shared_channel = ChannelPtr(channel);
        // 
        //activeList.insert(std::make_pair(channel->getChannelFd(), channel));
        activeList.push_back(channel);
    }
    return num_events;
}

Epoll::~Epoll()
{

}


