#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAX 1024

int Getline(int client_sock, char buf[], int size)
{
    char ch = 'a';
    int i = 0;
    ssize_t s = 0;
    while(ch != '\n' && i < size-1 )
    {
        s = read(client_sock,&ch,1);
        if(s > 0){
            buf[i++] = ch;                           
        }else
        {
            break;
        }
    }
    buf[i] = '\0';
    return i;
}
int ClearHeader(int sock)
{
    int ret = 0;
    char line[MAX];
    do{
        int s = 0;
        if((s = Getline(sock,line,MAX)) <= 0)
            break;
        ret += s;
    }while(strcmp(line,"\r\n") != 0);
    return ret;
}

int main()
{
    char method[MAX/32],query_string[MAX*1000];
    int content_length = 0;
    int new_content_length = 0;
    strcpy(method,getenv("METHOD"));
    if(strcasecmp(method,"GET") == 0)
    {
        strcpy(query_string,getenv("QUERY_STRING"));
    }else if(strcasecmp(method,"POST") == 0)
    {
        content_length = atoi(getenv("CONTENT_LENGTH"));
        int i = ClearHeader(0);
        content_length -= i;
        new_content_length = content_length;
        char filename[32];
        sprintf(filename,"/home/wh/31class/network/new_item/my_http_item/user_upload_picture/%s.jpg",getenv("CONTENT_LENGTH"));
        // int sum = 0;
        // char line[MAX*100];
        int fd = open(filename,O_RDWR | O_CREAT ,S_IRUSR | S_IRGRP |S_IROTH );
        char ch;
        for(i = 0 ; i < new_content_length ; i++)
        {
            read(0,&ch,1);
            write(fd,&ch,1);
        }
        // sum = Getline(0,line,sizeof(line));
        // while(strncmp(line,"------Web",9) != 0)
        // {
        //     printf("%s",line);
        //     write(fd,line,sum);
        //     sum += Getline(0,line,sizeof(line));
        // }
        close(fd);
        printf("<html>");
        printf("<body>");
        printf("<body background=\"%s\">",filename);
        printf("</body>");
        printf("</body>");
        printf("</html>");
    }
    return 0;
}

