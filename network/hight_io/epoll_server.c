#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <string.h>
#define MAXEVENTS 10

//打印提示手册
void Usage()
{
    printf("Usage: ./epoll_server.c [port]\n");
    exit(0);
}

//创建监听套接字
int StartUp(int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if(bind(sock,(struct sockaddr*)&server, sizeof(server)) < 0)
    {
        perror("bind");
        exit(2);
    }

    if(listen(sock,5) < 0)
    {
        perror("listen");
        exit(3);
    }
    return sock;
}
//服务函数
void Server(int listen_sock, int epoll_fd, struct epoll_event re_epoll_event[], int num)
{
    int i = 0;
    for(;i < num;++i)
    {
        if(re_epoll_event[i].events & EPOLLIN){
            if(re_epoll_event[i].data.fd == listen_sock && re_epoll_event[i].events & EPOLLIN)
            {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                int client_sock = accept(re_epoll_event[i].data.fd,(struct sockaddr*)&client,&len);
                if(client_sock < 0)
                {
                    perror("accept");
                    continue;
                }
                printf("Client[%s] accept success!\n",inet_ntoa(client.sin_addr));
                struct epoll_event event;
                event.data.fd = client_sock;
                event.events = EPOLLIN | EPOLLOUT;
                epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_sock,&event);
                continue;
            }
            else
            {
                char buf[1024];
                buf[0] = '\0';
                ssize_t s = recv(re_epoll_event[i].data.fd,buf,sizeof(buf)-1,0);
                if(s > 0)
                {
                    buf[s] = '\0';
                    printf("Client> %s\n",buf);
                    send(re_epoll_event[i].data.fd,buf,strlen(buf),0);
                    continue;
                }else if(s == 0)
                {
                    printf("Client quit\n");
                    epoll_ctl(epoll_fd,EPOLL_CTL_DEL,re_epoll_event[i].data.fd,NULL);
                    close(re_epoll_event[i].data.fd);
                    continue;
                }
                else
                {
                    perror("recv");
                    continue;
                }
            }
        }
    }
}

int main(int argc, char* argv[])
{
    if(argc != 2)
        Usage();
    int listen_sock = StartUp(atoi(argv[1]));

    //创建epoll模型
    int epoll_fd = epoll_create(MAXEVENTS+1);
    if(epoll_fd == -1)
    {
        perror("epoll_create");
        return 1;
    }
    //向epoll模型中添加文件描述符并注册事件
    struct epoll_event epoll_event;
    epoll_event.events = EPOLLIN;
    epoll_event.data.fd = listen_sock;

    //接收epoll_wait返回值
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock,&epoll_event ) < 0)
    {
        perror("epoll_ctl");
        return 2;
    }
    for(;;)
    {
        struct epoll_event re_epoll_event[MAXEVENTS];
        int ret = epoll_wait(epoll_fd,re_epoll_event,MAXEVENTS,-1);
        switch(ret)
        {
        case 0:
            printf("epoll_wait timeout...\n");
            break;
        case -1:
            perror("epoll_wait");
            break;
        default:
            Server(listen_sock,epoll_fd,re_epoll_event,ret);
            break;
        }
    }
    return 0;
}
