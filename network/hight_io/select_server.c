#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>

#define MAX_FD sizeof(fd_set)*8  //需要进行等待的文件描述符数组最大值
#define INIT -1                  //文件描述符数组初始化值

//文件描述符数组初始化
void AddryInit(int array[], int num )
{
    int i = 0;
    for(; i < num; ++i)
    {
        array[i] = INIT;
    }
}
//往文件描述符数组中添加新的需要等待的文件描述符
void AddryAdd(int array[], int num ,int fd)
{
    int i = 0;
    for(; i < num; ++i)
    {
        if(array[i] == INIT)
        {
            array[i] = fd;
            return;
        }
    }
}
//往可读文件描述符集中添加文件描述符
int ReadFdSet(int listen_sock, int array[], int num , fd_set* read_set)
{
    int i = 0;
    FD_SET(listen_sock, read_set);
    int max = listen_sock;
    for(; i < num; ++i)
    {
        if(array[i] != INIT)
        {
            FD_SET(array[i], read_set);
            if(max < array[i])
            {
                max = array[i];
            }
        }
    }
    return max;
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
//从文件描述符数组中删除文件描述符
void AddryDel(int array[], int num, int i)
{
    if(i < num)
    {
        array[i] = INIT;
    }
}
//服务函数
void Service(int listen_sock,fd_set* read_set, int array[],int num)
{
    int i = 0;
    for(; i < num ; ++i)
    {
        if(array[i] != INIT && FD_ISSET(array[i],read_set))
        {
            if(array[i] == listen_sock)
            {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                int client_sock = accept(listen_sock,(struct sockaddr*)&client, &len);
                if(client_sock < 0)
                {
                    perror("accept");
                    continue;
                }
                AddryAdd(array,num,client_sock);
            }
            else
            {
                char buf[1024];
                ssize_t s = read(array[i], buf, sizeof(buf)-1);
                if(s > 0)
                {
                    buf[s] = 0;
                    printf("Client> %s\n",buf);
                }else if(s == 0)
                {
                    printf("client quit...\n");
                    AddryDel(array,num,i);
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

    int array[MAX_FD];
    AddryInit(array,MAX_FD);
    AddryAdd(array,MAX_FD,listen_sock);

    fd_set read_set;
    FD_ZERO(&read_set);

    struct timeval timeout = {5,0};
    for(;;)
    {
        int max = ReadFdSet(listen_sock,array, MAX_FD, &read_set);
        switch(select(max+1,&read_set,NULL,NULL,&timeout))
        {
        case 0:
            sleep(2);
            printf("select timeout...\n");
            break;
        case -1:
            perror("select");
            break;
        default:
            Service(listen_sock,&read_set,array,MAX_FD);
            break;
        }
    FD_ZERO(&read_set);
    }
}
