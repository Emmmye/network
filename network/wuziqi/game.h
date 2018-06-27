#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ROWS 10
#define COLS 10
typedef struct Point{
    int row;
    int col;
}Point;
//初始化棋盘
void InitBoard(char board[ROWS][COLS],int row, int col);

//打印棋盘
void PrintBoard(char board[ROWS][COLS], int row, int col);

//共同移动
void CommonMove(char board[ROWS][COLS],Point* point,char ch);

//客户端移动
void ClientMove(char board[ROWS][COLS],Point* point);

//服务器移动
void ServerMove(char board[ROWS][COLS],Point* point);

//判断输赢
char GameState(char board[ROWS][COLS],Point* point);


