#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
void SocketCommon(int sock, struct sockaddr_in* server)
{
    char buf[1024];
    struct sockaddr_in peer;
    while(1)
    {
        socklen_t len = sizeof(peer);
        printf("Please Enter# ");
        fflush(stdout);
        ssize_t s = read(0,buf,sizeof(buf)-1);
        if(s > 0)
        {
            buf[s-1] = 0;
            sendto(sock,buf,strlen(buf),0,(struct sockaddr*)server,sizeof(*server));
//            ssize_t _s = recvfrom(sock,buf,sizeof(buf)-1,0,(struct sockaddr*)&peer,&len);
//            if(_s > 0)
//            {
//                buf[_s] = 0;
//                printf("server echo# %s\n",buf);
//            }
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

    struct sockaddr_in server;
    server.sin_family = AF_INET;//ipv4地址类型
    server.sin_port = htons(atoi(argv[2]));//端口号
    server.sin_addr.s_addr = inet_addr(argv[1]);//ipv4地址转换 因为输入的是点分十进制
    SocketCommon(sock,&server);
    return 0;
}
