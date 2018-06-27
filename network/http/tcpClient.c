#include <stdio.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 128

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Usage: [ip] [port]\n");
        exit(1);
    }
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(2);
    }
    
    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(argv[1]);
    client.sin_port = htons(atoi(argv[2]));

    socklen_t len = sizeof(client);
    if(connect(sock,(struct sockaddr*)&client,len) < 0)
    {
        perror("connect");
        exit(3);
    }

    char buf[MAX];
    printf("connect success....\n");
    while(1)
    {
        printf("Client say#");    
        buf[0] = 0;
        fflush(stdout);
        ssize_t s = read(0,buf,sizeof(buf));
        if(s > 0)
        {
            buf[s-1] = 0;
            write(sock,buf,strlen(buf));
            if(strcmp("quit",buf) == 0)
            {
                printf("connect quit...\n");
                close(sock);
                return 0;
            }
//            read(sock,buf,sizeof(buf));
            fflush(stdout);
//            printf("Server reply#%s \n",buf);
        }
    }

    close(sock);

    return 0;
}
