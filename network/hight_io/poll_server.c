#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/socket.h>

//将poll_fd结构体存储到一个数组中
#define MAX_FD 1024  //需要进行等待的文件描述符数组最大值
#define INIT -1                  //文件描述符数组初始化值
//poll_fd数组初始化
void Init(struct pollfd* fd_list, int num )
{
    int i = 0;
    for(; i < num; ++i)
    {
        fd_list[i].fd = INIT;
        fd_list[i].events = 0;
        fd_list[i].revents = 0;
    }
}
//往文件描述符数组中添加新的需要等待的文件描述符
void Add(struct pollfd* fd_list, int num ,int fd)
{
    int i = 0;
    for(; i < num; ++i)
    {
        if(fd_list[i].fd == INIT)
        {
            fd_list[i].fd = fd;
            fd_list[i].events = POLLIN;
            return;
        }
    }
}
//绑定端口号，将套接字设置为监听状态
int StartUp(int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = INADDR_ANY;
    socklen_t len = sizeof(local);
    if(bind(sock,(struct sockaddr*)&local, len) < 0)
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
void Service(int listen_sock,struct pollfd* fd_list,int num)
{
    int i = 0;
    for(; i < num ; ++i)
    {
        if(fd_list[i].fd != INIT && fd_list[i].revents & POLLIN )//对应的文件描述符上的读事件就绪
        {
            if(fd_list[i].fd == listen_sock)
            {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                int client_sock = accept(listen_sock,(struct sockaddr*)&client, &len);
                if(client_sock < 0)
                {
                    perror("accept");
                    continue;
                }
                Add(fd_list,MAX_FD,client_sock);
            }
            else
            {
                char buf[1024];
                ssize_t s = read(fd_list[i].fd, buf, sizeof(buf)-1);
                if(s > 0)
                {
                    buf[s] = 0;
                    printf("Client> %s\n",buf);
                }else if(s == 0)
                {
                    printf("client quit...\n");
                    close(fd_list[i].fd);
                    fd_list[i].fd = INIT;
                }else
                {
                    perror("read");
                }
            }
        }
    }
}
int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        printf("Usage: [%s] [port]\n",argv[0]);
        return 1;
    }
    
    int listen_sock = StartUp(atoi(argv[1]));

    struct pollfd fd_list[MAX_FD];
    Init(fd_list,MAX_FD);
    Add(fd_list,MAX_FD,listen_sock);
    
    for(;;)
    {
        switch(poll(fd_list,MAX_FD,1000))
        {
        case 0:
            sleep(2);
            printf("select timeout...\n");
            break;
        case -1:
            perror("select");
            break;
        default:
            Service(listen_sock,fd_list,MAX_FD);
            break;
        }
    }
}
