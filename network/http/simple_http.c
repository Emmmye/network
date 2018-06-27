#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define MAX 10240

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Usage:./server [ip] [port]\n");
        exit(1);
    }
    //创建服务端TCP套接字
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        perror("socket");
        exit(2);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));
    
    //绑定端口号
    int ret = bind(sock,(const struct sockaddr*)&server,sizeof(server));
    if(ret < 0)
    {
        perror("bind");
        exit(3);
    }
    int opt = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    //监听
    ret = listen(sock,5);
    if(ret < 0)
    {
        perror("listen");
        exit(4);
    }
    printf("Listening...\n");
    for(;;)
    {
        struct sockaddr_in client;
        socklen_t len;
        int client_sock = accept(sock,(struct sockaddr*)&client,&len);
        if(client_sock < 0)
        {
            continue;
        }
        printf("Content success...\n");
        char input_buf[MAX];//用一个足够大的缓冲区直接将数据读走
        ssize_t read_size = read(client_sock,input_buf,sizeof(input_buf)-1);
        if(read_size < 0)
        {
            printf("Connect close...\n");
            close(sock);
        }
        char buf[1024] = {0};
        printf(" %s ",input_buf);
        const char* hello = "<html><body><h1>hello world</h1></body></html>";
        sprintf(buf, "HTTP/1.0 200 OK \r\n  \r\n%s",hello);
        write(client_sock,buf,strlen(buf));
    }
    
    return 0;
}
