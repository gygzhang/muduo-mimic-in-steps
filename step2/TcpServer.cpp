#include"TcpServer.h"

using namespace std;

TcpServer::TcpServer()
: max_events_(512), max_listenfd_(5)
{
    events_.reserve(max_events_);
}

TcpServer::TcpServer(int max_events, int max_listenfd)
: max_events_(max_events), max_listenfd_(max_listenfd)
{
    events_.reserve(max_events_);
}

void TcpServer::listenOn(int port)
{
    int on=1;
    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    /* allow immediate reuse of the port */
    fcntl(listenfd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(on));
    servaddr.sin_family = AF_INET;
    in_addr idr;
    //inet_aton("0.0.0.0", &idr);
    //inet_pton
    //servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if(bind(listenfd_, (sockaddr*)&servaddr, sizeof(servaddr))<0){
        cout<<"bind: "<<errno<<endl;
    }
    if(listen(listenfd_, MAX_LISTENFD)<0){
        cout<<"listen"<<endl;
    }
}


void TcpServer::start(int port)
{
    listenOn(port);
    epollfd_ = epoll_create(1);
    struct epoll_event ev;
    ev.data.fd = listenfd_;
    ev.events = EPOLLIN;
    epoll_ctl(epollfd_, EPOLL_CTL_ADD, listenfd_, &ev);
    for(;;)
    {
        int num_events = epoll_wait(epollfd_, (epoll_event*)events_.data(), max_events_, -1);
        if(num_events<0) {
            cout<< "num_events: "<< errno<<endl;
            break;
        }
        for (int i = 0; i < num_events; i++)
        {
            if(events_[i].data.fd==listenfd_){
                struct sockaddr_in cli_addr1;
                socklen_t cli_len;
                int fd = accept(listenfd_, (sockaddr*)&cli_addr1, &cli_len);
                if(fd>0){
                    cout<<"\nnew connection from "
                        <<"["<<inet_ntoa(cli_addr1.sin_addr)
                        <<":"<<ntohs(cli_addr1.sin_port)
                        <<"#"<<fd<<"] accepted!"<<endl; 
                }else{
                    cout<<"accept error: "<<errno<<endl;
                }
                fcntl(fd, F_SETFL, O_NONBLOCK);
                ev.data.fd = fd;
                ev.events = EPOLLIN;
                if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev)<0){
                    cerr<<"ctl error: "<<errno<<endl; 
                }
            }else if(events_[i].events & EPOLLIN){
                struct sockaddr_in addr;
                socklen_t socklen = sizeof(struct sockaddr_in);
                int rdfd = events_[i].data.fd;
                int res = getsockname(rdfd, (struct sockaddr*)&addr, &socklen);
                assert(res>=0);
                if(rdfd<0){
                    cout<<"rdfd<0: "<<errno<<endl;
                    continue;
                }
                char buf[BUFFER_SIZE];
                int nbytes=0;
                nbytes = read(rdfd, buf, BUFFER_SIZE);
                assert(nbytes>=0);
                if(nbytes<0){
                    if(errno==ECONNRESET){
                        cerr << "ECONNREST closed socket fd:" << events_[i].data.fd << endl;
                        close(rdfd);
                    }
                }else if(nbytes==0){
                    cerr<<"connection closed by peer. [ server host: "<<inet_ntoa(addr.sin_addr)
                        <<":"<<ntohs(addr.sin_port)<<"#"<<rdfd<<"]"<<endl;
                    close(rdfd);
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
    }
}


