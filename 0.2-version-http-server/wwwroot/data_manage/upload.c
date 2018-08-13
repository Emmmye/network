#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "/home/wh/31class/network/0.1-version-http-server/lib/include/mysql.h"
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

void InsertIntoMySQL(char* name, char* img)
{
    //初始化
    MYSQL *my_sql = NULL;
    my_sql = mysql_init(NULL);
    if(!mysql_real_connect(my_sql, "127.0.0.1", "root", "", "myhttp", 3306, NULL, 0) ){
        printf( "<h1>FAIL</h1>"  );
        return ;                     
    }
    //创建SQL语句
    char words[MAX];
    sprintf(words,"insert into pic (name,img) values(\"%s\",\"%s\")",name,img);
    if(mysql_query(my_sql,words) != 0)
    {
        printf("<h1>INSERT FAIL</h1>");
    }
    //关闭数据库连接
    mysql_close(my_sql);
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
        sprintf(filename,"/home/wh/31class/network/0.1-version-http-server/user_upload_picture/%s.jpg",getenv("CONTENT_LENGTH"));
        // int sum = 0;
        // char line[MAX*100];
        int fd = open(filename,O_RDWR | O_CREAT ,S_IRUSR | S_IRGRP |S_IROTH );
        char ch;
        for(i = 0 ; i < new_content_length ; i++)
        {
            read(0,&ch,1);
            write(fd,&ch,1);
        }
        close(fd);
        char name[128],img[128];
        sprintf(img,"../../user_upload_picture/%s.jpg",getenv("CONTENT_LENGTH"));
        sprintf(name,"%s",getenv("CONTENT_LENGTH"));
        InsertIntoMySQL(name,img);
        
        printf("<html>");
        printf("<meta http-equiv=\"refresh\" content=\"1,url=../upload.html\">");
        printf("<meta charset=\"utf-8\"><style>");
        printf("body{text-align:center;margin-left:auto;margin-right:auto;}");
        printf("</style>");
        printf("<body>");
        printf("<h1>上传成功!感谢您的分享!</h1></br>");
        printf("即将为您跳转</br>");
        printf("</body>");
        printf("</html>");
    }
    return 0;
}

