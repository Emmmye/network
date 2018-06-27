#include "game.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>  
#include <netinet/in.h>
#include <unistd.h>

//初始化棋盘
void InitBoard(char board[ROWS][COLS],int row, int col)
{
    memset(board,' ',row*col*sizeof(char));
}
//打印棋盘
void PrintBoard(char board[ROWS][COLS],int row,int col)
{
    int  i = 0;
    printf("    1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10\n");
    for(; i < row;i++)
    {
        printf("%2d ",i+1);
        int j = 0;
        for(; j < col;j++)
        {
            if( j == col-1 )
            {
                printf(" %c \n",board[i][j]);
                if(i != row-1)
                    printf("   ---|---|---|---|---|---|---|---|---|---\n");
                break;
            }
            printf(" %c ",board[i][j]);
            printf("|");
        }

    }
}

//判断棋盘是否满了
int IsFall(char board[ROWS][COLS])
{
    size_t i = 0;
    for(; i < ROWS;i++)
    {
        size_t j = 0;
        for(; j < COLS; j++)
        {
            if(board[i][j] == ' ')
                return 0;
        }
    }
    return 1;
}
//移动
void CommonMove(char board[ROWS][COLS],Point* point,char ch)
{
    if(point == NULL)
    {
        return;
    }
    int x = point->row;
    int y = point->col;
    //如果棋盘满了的话就退出循环，也就是不能再下子了
    while(1)
    {
        if(x >= 0 && x < ROWS && y >= 0 && y < COLS){
            if(board[x][y] == ' ')
            {
                board[x][y] = ch;
                point->row = x;
                point->col = y;
                break;
            }
            else
            {
                printf("坐标输入有误,请重新输入坐标: ");
                scanf("%d%d",&x,&y);
                x--;
                y--;
            }
        }
        else{
            printf("坐标输入有误,请重新输入坐标: ");
            scanf("%d%d",&x,&y);
            x--;
            y--;
        }
    }
}
//客户端移动
void ClientMove(char board[ROWS][COLS],Point* point)
{
    CommonMove(board,point,'X');
}

//服务器移动
void ServerMove(char board[ROWS][COLS],Point* point)
{
    CommonMove(board,point,'O');
}
//列齐
int ColState(char board[ROWS][COLS],Point* point)
{
    int x = point->row;
    int y = point->col;
    int count = 1;
    while(x-1 >= 0)
    {
        if(board[x][y] != board[x-1][y])
            break;
        count++;
        if(count == 5)
        {
            //如果往上遍历已经够了五个，说明已经赢了，直接返回
            return count;
        }
        x--;
    }
    //到这里说明当前点往上走是不满足条件的
    //但是已经走到了最上面的点，所以从临界点往相反方向找
    //必须要找到五个连续的才算满足条件
    count = 1;//count 重新被设定为1
    while(x+1 <= ROWS)
    {
        if(board[x][y] == board[x+1][y])
        {
            count++;
            if(count == 5)
            {
                return count;
            }
            x++;
        }
        else
        {
            //n 代表当前状态未满足赢的条件
            return 0;
        }
    }
    return 0;
}
//行齐
int RowState(char board[ROWS][COLS],Point* point)
{
    int x = point->row;
    int y = point->col;
    int count = 1;
    while(y-1 >= 0)
    {
        if(board[x][y] != board[x][y-1])
            break;
        count++;
        if(count == 5)
        {
            //如果往左遍历已经够了五个，说明已经赢了，直接返回
            return count;
        }
        y--;
    }
    //到这里说明当前点往左走是不满足条件的
    //但是已经走到了最左面的点，所以从临界点往相反方向找
    //必须要找到五个连续的才算满足条件
    count = 1;//count 重新被设定为1
    while(y+1 <= COLS)
    {
        if(board[x][y] == board[x][y+1])
        {
            count++;
            if(count == 5)
            {
                return count;
            }
            y++;
        }
        else
        {
            //n 代表当前状态未满足赢的条件
            return 0;
        }
    }
    return 0;
}
//左上到右下的对角线
int UpLeftState(char board[ROWS][COLS],Point* point)
{
    int x = point->row;
    int y = point->col;
    int count = 1;
    while(x-1 >= 0 && y-1 >= 0 )
    {
        if(board[x][y] != board[x-1][y-1])
            break;
        count++;
        if(count == 5)
        {
            //如果往左上遍历已经够了五个，说明已经赢了，直接返回
            return count;
        }
        x--;
        y--;
    }
    //到这里说明当前点往左上走是不满足条件的
    //但是已经走到了最左上的点，所以从临界点往相反方向找
    //必须要找到五个连续的才算满足条件
    count = 1;//count 重新被设定为1
    while(x+1 <= ROWS && y+1 <= COLS)
    {
        if(board[x][y] == board[x+1][y+1])
        {
            count++;
            if(count == 5)
            {
                return count;
            }
            x++;
            y++;
        }
        else
        {
            //n 代表当前状态未满足赢的条件
            return 0;
        }
    }
    return 0;
}
//右上到左下的对角线
int UpRightState(char board[ROWS][COLS], Point* point)
{
    int x = point->row;
    int y = point->col;
    int count = 1;
    while(x-1 >= 0 && y+1 <= COLS)
    {
        if(board[x][y] != board[x-1][y+1])
            break;
        count++;
        if(count == 5)
        {
            //如果往右上遍历已经够了五个，说明已经赢了，直接返回
            return count;
        }
        x--;
        y++;
    }
    //到这里说明当前点往右上走是不满足条件的
    //但是已经走到了最右上面的点，所以从临界点往相反方向找
    //必须要找到五个连续的才算满足条件
    count = 1;//count 重新被设定为1
    while(x+1 <= ROWS && y-1 <=COLS)
    {
        if(board[x][y] == board[x+1][y-1])
        {
            count++;
            if(count == 5)
            {
                return count;
            }
            x++;
            y--;
        }
        else
        {
            //n 代表当前状态未满足赢的条件
            return 0;
        }
    }
    return 0;
}
//返回游戏进行状态
char GameState(char board[ROWS][COLS],Point* point)
{
    if(point == NULL)
    {
        return 'e';
    }
    //判断当前点所在列是否是连续五个子儿
    if(ColState(board,point) == 5)
    {
        return board[point->row][point->col];
    }
    //判断当前点所在行是否是连续五个子儿
    else if(RowState(board,point) == 5)
    {
        return board[point->row][point->col];
    }
    //判断当前点所在左上与右下的对角线是否满足条件
    else if(UpLeftState(board,point) == 5)
    {
        return board[point->row][point->col];
    }
    //判断当前点所在左上与右下的对角线是否满足条件
    else if(UpRightState(board,point) == 5)
    {
        return board[point->row][point->col];
    }
    else if(IsFall(board))
    {
        //平局
        return 'p'; 
    }
    //当前没有输赢，继续游戏
    return 'g';
}

////////////////////////////////////////////
////////TEST  PART
///////////////////////////////////////////


