#include"TcpServer.h"

using namespace std;
using namespace std::placeholders;

TcpServer::TcpServer()
: _max_events(512), _max_listenfd(5)
{
    _events.reserve(_max_events);
    //_pChannel = std::make_unique<Channel>();
}

TcpServer::TcpServer(int max_events, int max_listenfd)
: _max_events(max_events), _max_listenfd(max_listenfd)
{
    _events.reserve(_max_events);
}

void TcpServer::onConnection()
{

}

void TcpServer::onEvent(int fd)
{
    cout<<"onEvent: "<<fd<<endl;
    if(fd==_listenfd){
        struct sockaddr_in cli_addr1;
        socklen_t cli_len = sizeof(struct sockaddr_in);
        int peerfd = accept(_listenfd, (sockaddr*)&cli_addr1, &cli_len);
        if(peerfd>0){
            cout<<"\nnew connection from "
                <<"["<<inet_ntoa(cli_addr1.sin_addr)
                <<":"<<ntohs(cli_addr1.sin_port)
                <<"#"<<peerfd<<"] accepted!"<<endl; 
        }else{
            cout<<"accept error: "<<errno<<endl;
        }

        fcntl(peerfd, F_SETFL, O_NONBLOCK);

        Channel *channel = new Channel(_epollfd, peerfd);
        //once channel readable, it will call onEvent
        channel->setReadCallBack(std::bind(&TcpServer::onEvent, this, std::placeholders::_1));
        channel->enableReading();
        _channels.insert(std::make_pair(peerfd, channel));
        // ev.data.fd = fd;
        // ev.events = EPOLLIN;
        // if(epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev)<0){
        //     cerr<<"ctl error: "<<errno<<endl; 
        // }
    }else {
        struct sockaddr_in addr;
        socklen_t socklen = sizeof(struct sockaddr_in);
        int rdfd = fd;
        int res = getsockname(rdfd, (struct sockaddr*)&addr, &socklen);
        assert(res>=0);
        if(rdfd<0){
            cout<<"rdfd<0: "<<errno<<endl;
            return;
        }
        char buf[BUFFER_SIZE];
        int nbytes=0;
        nbytes = read(rdfd, buf, BUFFER_SIZE);
        assert(nbytes>=0);
        if(nbytes<0){
            if(errno==ECONNRESET){
                cerr << "ECONNREST closed socket fd:" << fd << endl;
                close(rdfd);
            }
        }else if(nbytes==0){
            cerr<<"connection closed by peer. [ server host: "<<inet_ntoa(addr.sin_addr)
                <<":"<<ntohs(addr.sin_port)<<"#"<<rdfd<<"]"<<endl;
            close(rdfd);
            //ChannelPtr pa = _channels.at(rdfd);
            //auto it = _channels.find(rdfd);
            //(*it).second.~unique_ptr();
            //ChannelPtr p = 
            //_channels.erase(rdfd);
        }else{
            cout<<"recv from client: "<<buf<<endl;
            memset(buf,0,sizeof(buf));
            snprintf(buf, BUFFER_SIZE, "from server\n");
            if(write(rdfd, buf, nbytes)!=nbytes){
                cerr<<"write not finish at once. [ server host: "<<inet_ntoa(addr.sin_addr)
                <<":"<<ntohs(addr.sin_port)<<"#"<<rdfd<<"]"<<endl;
            }
        }
    }
}

void TcpServer::listenOn(int port)
{
    int on=1;
    _listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    /* allow immediate reuse of the port */
    fcntl(_listenfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(on));
    servaddr.sin_family = AF_INET;
    in_addr idr;
    //inet_aton("0.0.0.0", &idr);
    //inet_pton
    //servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if(::bind(_listenfd, (sockaddr*)&servaddr, sizeof(servaddr))<0){
        cout<<"bind: "<<errno<<endl;
    }
    if(listen(_listenfd, MAX_LISTENFD)<0){
        cout<<"listen"<<endl;
    }
}


void TcpServer::start(int port)
{
    listenOn(port);
    _epollfd = epoll_create(1);

    // listening channel
    _pChannel = make_unique<Channel>(_epollfd, _listenfd);
    _pChannel->setReadCallBack(std::bind(&TcpServer::onEvent,this, _1));
    _pChannel->enableReading();

    struct epoll_event ev;
    //ev.data.fd = _listenfd;
    ev.data.ptr = _pChannel.get();
    ev.events = EPOLLIN;
    epoll_ctl(_epollfd, EPOLL_CTL_ADD, _listenfd, &ev);

    


    for(;;)
    {
        int num_events = epoll_wait(_epollfd, (epoll_event*)_events.data(), _max_events, -1);
        if(num_events<0) {
            break;
        }
        cout<<"num_events: "<<num_events<<endl;
        
        for (int i = 0; i < num_events; i++)
        {
            ChannelPtr channel = static_cast<ChannelPtr>(_events[i].data.ptr);
            //ChannelPtr shared_channel = ChannelPtr(channel);
            // do not using smart ptr to store channel, or it will causing double-deleting
            _activeChannel.insert(std::make_pair(channel->getChannelFd(), channel));
        }

        for(std::map<int, ChannelPtr>::const_iterator it = _activeChannel.cbegin();
            it!=_activeChannel.cend();
            it++)
        {
            (*it).second->handleEvent();
        }
        
        // clear the active channel list, prevents from double-handling
        
        _activeChannel.clear();

    }
}


