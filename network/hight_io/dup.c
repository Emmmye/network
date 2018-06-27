#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    int fd = open("./log",O_CREAT | O_RDWR);//在当前目录下创建一个可读可写的文件log
    close(1);
    int new_fd = dup(fd);
    printf("new_fd:%d\n",new_fd);
    int i = 10;
    while(i--)
    {
        printf("Hello\n");
    }
    return 0;
}
