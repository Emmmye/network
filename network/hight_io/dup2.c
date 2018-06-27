#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    int fd = open("./log", O_CREAT | O_RDWR);
    dup2(fd,1);
    int i = 10;
    while(i--)
    {
        printf("Hello\n");
    }
    return 0;
}
