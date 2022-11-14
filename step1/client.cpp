#include<sys/socket.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<iostream>
#include<unistd.h>
#include<assert.h>

#define MAXBYTES 512
using namespace std;
int main(){
    sockaddr_in sock;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(fd>=0);
    //101.33.214.121
    //127.0.0.1
    int res = inet_pton(AF_INET,"127.0.0.1", &sock.sin_addr);
    assert(res>=0);
    sock.sin_family = AF_INET;
    sock.sin_port = htons(12345);
    socklen_t socklen = sizeof(struct sockaddr_in);


    int r = connect(fd,(struct sockaddr*)&sock, socklen);
    assert(r>=0);
    char buf[MAXBYTES];
    sockaddr_in local_addr;
    socklen_t local_sock_len = sizeof(struct sockaddr_in);
    //getsockname(fd,(struct sockaddr*)&local_addr, &local_sock_len);
    getpeername(fd,(struct sockaddr*)&local_addr, &local_sock_len);
    snprintf(buf,MAXBYTES,"msg to %s : I will close after 5 seconds.\n", inet_ntoa(local_addr.sin_addr));
    cout<<buf<<endl;
    sleep(1);
    send(fd, buf, strlen(buf), 0);
    memset(buf,0, sizeof(buf));
    read(fd, buf,MAXBYTES);
    cout<<"recv: "<<buf<<endl;
    sleep(1);
    close(fd);

}