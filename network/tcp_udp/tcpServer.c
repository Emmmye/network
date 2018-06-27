#include <stdio.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#define MAX 128 

typedef struct Arg{
    int fd;
    struct sockaddr_in addr;
}Arg;

void* Server(void *arg)
{
    Arg* p = (Arg*)arg;
    char buf[MAX];
    while(1)
    {
        buf[0] = 0;
        ssize_t s = read(p->fd,buf,sizeof(buf));
        if(s < 0)
        {
            perror("read");
            continue;
        }else if(s == 0)
        {
            printf("client[%s] quit!\n",inet_ntoa(p->addr.sin_addr));
            close(p->fd);
            break;
        } 
        buf[s] = 0;
        printf("client[%s] says:%s\n",inet_ntoa(p->addr.sin_addr),buf);
        write(p->fd,buf,strlen(buf));
    }
    return NULL;
}


int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Usage: [ip] [port]\n");
        exit(1);
    
    }
    int sock = socket(AF_INET,SOCK_STREAM,0);//socket处于创建状态
    if(sock < 0)
    {
        perror("socket");
        exit(2);
    }
    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);
    
    if(bind(sock,(const struct sockaddr*)&server,sizeof(server)) < 0)
    {
        perror("bind");
        exit(3);
    }

    listen(sock,5);
    if(sock < 0)//此时处于监听状态
    {
        perror("listen");
        exit(4);
    }
    printf("Bind and listen successed ,waiting for accepting...\n");

    for(;;)
    {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int client_socket = accept(sock,(struct sockaddr*)&client,&len);
        printf("。。。\n");
        if(client_socket < 0)
        {
            continue;
        }
        pthread_t thread;
        Arg* arg;
        arg = (Arg*)malloc(sizeof(arg));
        arg->fd = client_socket;
        arg->addr = client;
        if(pthread_create(&thread,NULL,Server,(void*)arg) == 0)
        {
            perror("pthread_create");
        }
        pthread_detach(thread);
//        pid_t son = fork();
//        if(son < 0)
//        {
//            perror("fork");
//            return 0;
//        }
//        if(son == 0)
//        {
//            pid_t son_son = fork();
//            if(son_son == 0)
//            {
//                Server(client_socket,&client);
//            }
//            exit(0);
//        }
//        wait(NULL);
    }
    return 0;
}
