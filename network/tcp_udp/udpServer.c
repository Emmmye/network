#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
void SocketCommon(int sock)
{
    char buf[1024];
    struct sockaddr_in client;
    while(1)
    {
        socklen_t len = sizeof(client);
        ssize_t s = recvfrom(sock,buf,sizeof(buf)-1,0,(struct sockaddr*)&client,&len);
        if(s > 0)
        {
            buf[s] = 0;
            printf("[%s:%d]:%s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),buf);
            fflush(stdout);
            sendto(sock,buf,strlen(buf),0,(struct sockaddr*)&client,sizeof(client));
        }
    }
}

int main(int argc,char *argv[])
{
    if(argc < 3)
    {
        perror("argc");
        exit(1);
    }
    //创建 socket 文件描述符
    //AF_INET    ipv4地址类型
    //SOCK_DGRAM udp类型
    //0          默认使用SOCK_DGRAM类型
    int sock = socket(AF_INET,SOCK_DGRAM,0);
    if(sock < 0)
    {
        perror("socket");
        exit(2);
    }

    struct sockaddr_in local;//定义struct sockaddr_in结构体变量
    local.sin_family = AF_INET;//ipv4地址类型
    local.sin_port = htons(atoi(argv[2]));//端口号(首先要通过atoi函数将字符串转化成整数，再调整为网络字节序)
    local.sin_addr.s_addr = inet_addr(argv[1]);//ipv4地址转换 因为输入的是点分十进制(字符串)

    if(bind(sock,(const struct sockaddr*)&local,sizeof(local)) < 0)//绑定端口号
    {
        perror("bind");
        exit(3);
    }

    SocketCommon(sock);
    return 0;
}
