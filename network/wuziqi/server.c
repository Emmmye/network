#include "game.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>  
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Usage: ./server [ip] [port]\n");
        exit(1);
    }
    
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));

    int ret = bind(sock,(const struct sockaddr*)&server,sizeof(server));
    if(ret < 0)
    {
        perror("bind");
        exit(2);
    }
    ret = listen(sock,5);
    if(ret < 0)
    {
        perror("listen");
        exit(3);
    }

    for(;;)
    {
        struct sockaddr_in client;
        socklen_t len;
        int client_sock = accept(sock,(struct sockaddr*)&client,&len);
        if(client_sock < 0)
        {
            continue;
        }
        ssize_t s;
        Point point_server,point_client;
        char board[ROWS][COLS];
        InitBoard(board,ROWS,COLS);
        int x,y;
        while(1){
            s = read(client_sock,(void*)&point_client,sizeof(point_client));
            if(s == 0)
            {
                printf("Client quit...\n");
                close(client_sock);
            }
            ClientMove(board,&point_client);
            PrintBoard(board,ROWS,COLS);
            if(GameState(board,&point_client) == 'X')
            {
                printf("Client win!\n");
                break;
            }
            else if(GameState(board,&point_client) == 'p')
            {
                printf("平局!\n");
                break;
            }
            printf("请下子(输入坐标)> ");
            scanf("%d%d",&x,&y);
            point_server.row = x-1;
            point_server.col = y-1;
            ServerMove(board,&point_server);
            PrintBoard(board,ROWS,COLS);
            write(client_sock,(void*)&point_server,sizeof(point_server));
            if(GameState(board,&point_client) == 'O')
            {
                printf("You win!\n");
                break;
            }
            else if(GameState(board,&point_client) == 'p')
            {
                printf("平局!\n");
                break;
            }

        }//游戏下完了
    }
    close(sock);
    return 0;
}
