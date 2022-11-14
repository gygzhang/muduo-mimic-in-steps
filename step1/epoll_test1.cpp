#include<sys/socket.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<iostream>
#include<unistd.h>
#include<assert.h>

using namespace std;

#define MAX_LINE 100
#define MAX_EVENTS 500
#define MAX_LISTENFD 5

int listen_on(int port)
{
    int on=1;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    /* allow immediate reuse of the port */
    fcntl(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if(bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr))<0){
        cout<<"bind: "<<errno<<endl;
    }
    if(listen(listenfd, MAX_LISTENFD)<0){
        cout<<"listen"<<endl;
    }
    return listenfd;
}

int main(int argc, char** argv)
{
    struct epoll_event ev, events[MAX_EVENTS];
    int listenfd, connfd, sockfd;
    int read_len;
    char line[MAX_LINE];
    struct sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(struct sockaddr_in);
    int epoll_fd = epoll_create(1);
    if(epoll_fd<0){
        cout<<"epoll_fd<0"<<endl;
    }
    listenfd = listen_on(8081);

    ev.data.fd = listenfd;
    ev.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listenfd, &ev);

    for(;;){
        //cout<<"num_events: "<<listenfd<<endl;
        int num_events = epoll_wait(epoll_fd,events, MAX_EVENTS,-1);
        //cout<<"num_events: "<<num_events<<endl;
        if(num_events<0) {
            cout<< "err: "<< errno<<endl;
            break;
        }
        for (int i = 0; i < num_events; i++)
        {
            if(events[i].data.fd==listenfd){
                struct sockaddr_in cli_addr1;
                socklen_t cli_len;
                int fd = accept(listenfd, (struct sockaddr*)&cli_addr1, &cli_len);
                if(fd>0){
                    cout<<"new connection from "
                        <<"["<<inet_ntoa(cli_addr1.sin_addr)
                        <<":"<<ntohs(cli_addr1.sin_port)
                        <<"#"<<fd<<"] accepted!"<<endl; 
                }else{
                    cout<<"accept error: "<<errno<<endl;
                }
                fcntl(fd, F_SETFL, O_NONBLOCK);
                ev.data.fd = fd;
                ev.events = EPOLLIN;
                if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev)<0){
                    cerr<<"ctl error: "<<errno<<endl;
                }
            }else if(events[i].events & EPOLLIN){
                struct sockaddr_in addr;
                socklen_t socklen = sizeof(struct sockaddr_in);
                int rdfd = events[i].data.fd;
                int res = getpeername(rdfd, (struct sockaddr*)&addr, &socklen);
                assert(res>=0);
                if(rdfd<0){
                    cout<<"rdfd<0: "<<errno<<endl;
                    continue;
                }
                char buf[MAX_LINE];
                int nbytes=0;
                nbytes = read(rdfd, buf, MAX_LINE);
                assert(nbytes>=0);
                if(nbytes<0){
                    if(errno==ECONNRESET){
                        cerr << "ECONNREST closed socket fd:" << events[i].data.fd << endl;
                        close(sockfd);
                    }
                }else if(nbytes==0){
                    cerr<<"connection closed by peer: ["<<inet_ntoa(addr.sin_addr)
                        <<":"<<ntohs(addr.sin_port)<<"#"<<rdfd<<"]"<<endl;
                    close(rdfd);
                }else{
                    cout<<"recv from client: "<<buf<<endl;
                    memset(buf,0,sizeof(buf));
                    snprintf(buf, MAX_LINE, "from server\n");
                    if(write(rdfd, buf, nbytes)!=nbytes){
                        cout<<"write not finish at once: ["<<inet_ntoa(addr.sin_addr)
                        <<":"<<ntohs(addr.sin_port)<<"#"<<rdfd<<"]"<<endl;
                    }
                }
            }
        }
        

    }

    //The device (normally a client) has sent a synchronize (SYN) message and is waiting for a matching SYN from the other

}



