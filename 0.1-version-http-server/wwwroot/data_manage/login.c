#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <errno.h>

#include "/home/wh/31class/network/0.1-version-http-server/lib/include/mysql.h"
#define MAX 1024
void Login(int id, int passward)
{
    //初始化
    MYSQL* my_sql = mysql_init(NULL);
    printf("<html>");
    printf("<body>");
    //连接数据库
    if(mysql_real_connect(my_sql,"127.0.0.1","root","","myhttp",3306,NULL,0) == NULL)
    {
        printf("<h3>连接数据库失败</h3>");
        return;
    }
    //执行SQL语句
    char words[32];
    sprintf(words,"select * from student where id =%d and passward =%d",id,passward);
    if(mysql_query(my_sql,words))
    {
        printf("<h4>查询失败</h4>");
        return;
    }
    //接收查询结果
    //该函数开辟了一段内存用来保存查询结果,后面一定要记得释放内存
    MYSQL_RES *result = mysql_store_result(my_sql);
    //获取行数
    my_ulonglong rows = mysql_num_rows(result);
    if(rows == 0)
    {
        printf("<h2>并未查找到您的信息,请查看密码以及学号是否填写正确<h2>");
        printf("<a href=\"../index.html\">返回登录界面进行修改</a>");
        printf("</html>");
        printf("</body>");
        return;
    }

    printf("<html>");
    printf("<meta http-equiv=\"refresh\" content=\"1,url=../upload.html\">");
    printf("<meta charset=\"utf-8\"><style>");
    printf("body{text-align:center;margin-left:auto;margin-right:auto;}");
    printf("</style>");
    printf("<body>");
    printf("<h1>登录成功!</h1></br>");
    printf("即将为您跳转!</br>");
    printf("</body>");
    printf("</html>");

    //     printf("<html>");
    //     printf("<body>");
    //     printf("<a href=\"../upload.html\">跳转至上传页面</a>");
    //     printf("</body>");
    //     printf("</html>");
//     int fd = open("../upload.html",O_RDONLY);
//     size_t s = read(fd,buf,sizeof(buf));
//     buf[s] = '\0';
//     write(1,buf,s);
//     close(fd);
    free(result);
    mysql_close(my_sql);
}

int main()    
{
    int content_length = 0;
    char method[MAX/100],query_string[MAX/2];
    strcpy(method,getenv("METHOD"));
    if(strcasecmp(method,"GET") == 0)
    {
        strcpy(query_string,getenv("QUERY_STRING"));
    }else if(strcasecmp(method, "POST") == 0)
    {
        content_length = atoi(getenv("CONTENT_LENGTH"));
        int i = 0;
        for(; i < content_length; ++i)
        {
            read(0,&query_string[i],1);
        }
    }
    char* id,*passward;
    strtok(query_string,"=&");
    id = strtok(NULL, "=&");
    strtok(NULL, "=&");
    passward = strtok(NULL,"=&");
    Login(atoi(id),atoi(passward));
    return 0;
}
