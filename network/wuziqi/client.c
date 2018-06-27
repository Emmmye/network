#include "game.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>  
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

void Menu()
{
    printf("==================================\n");
    printf("||  1.开始游戏   0.退出游戏     ||\n");
    printf("==================================\n");
}
void Game(int sock)
{
    Point point_server,point_client;
    char board[ROWS][COLS];
    InitBoard(board,ROWS,COLS);
    PrintBoard(board,ROWS,COLS);
    ssize_t s;
    int x,y;
    while(1){
        printf("请下子(输入坐标)> ");
        scanf("%d%d",&x,&y);
        point_client.row = x-1;
        point_client.col = y-1;
        ClientMove(board,&point_client);
        PrintBoard(board,ROWS,COLS);
        write(sock,(void*)&point_client,sizeof(point_client));
        if(GameState(board,&point_client) == 'X')
        {
            printf("You win!\n");
            break;
        }
        else if(GameState(board,&point_client) == 'p')
        {
            printf("平局!\n");
            break;
        }
        s = read(sock,(void*)&point_server,sizeof(point_server));
        if(s == 0)
        {
            printf("Server quit\n");
            return;
        }
        ServerMove(board,&point_server);
        PrintBoard(board,ROWS,COLS);
        if(GameState(board,&point_server) == 'O')
        {
            printf("Server win!\n");
            break;
        }
        else if(GameState(board,&point_server) == 'p')
        {
            printf("平局!\n");
            break;
        }
    }
}

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

    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(argv[1]);
    client.sin_port = htons(atoi(argv[2]));
    int ret = connect(sock,(const struct sockaddr*)&client,sizeof(client));
    if(ret < 0)
    {
        perror("connect");
        exit(2);
    }
    Menu();
    int state;
    while(1)
    {
        printf("请做出你的选择> ");
        scanf("%d",&state);
        if(state == 1)
        {
            Game(sock);
        }else if(state == 0)
        {
            break;
        }
        else
        {
            printf("选择错误\n");
            Menu();
            sleep(2);
        }
    }

    close(sock);
    return 0;
}
