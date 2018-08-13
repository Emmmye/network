#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "/home/wh/31class/network/0.2-version-http-server/lib/include/mysql.h"

#define MAX 1024
void SelectData(char *name)
{
    //初始化
    MYSQL* my_sql = mysql_init(NULL);
    //连接数据库
    if(mysql_real_connect(my_sql,"127.0.0.1","root","","myhttp",3306,NULL,0) == NULL)
    {
        printf("<html>");
        printf("<body>");
        printf("<h3>连接数据库失败</h3>");
        printf("<a href=\"../upload.html\">返回查询界面继续尝试</a>");
        printf("</html>");
        printf("</body>");
        return;
    }
    //执行SQL语句
    char words[32];
    char img[128];
    sprintf(words,"select * from pic where name =%s",name);
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
        printf("<html>");
        printf("<body>");
        printf("<h2>并未查找到此试卷信息,请查看试卷信息是否填写正确<h2>");
        printf("<a href=\"../upload.html\">返回查询界面进行修改</a>");
        printf("</body>");
        printf("</html>");
        return;
    }
    //获取列表内容
    MYSQL_ROW line;
    line =  mysql_fetch_row(result);
    //因为图片路径保存在第三列
    //所以就是查询结果的第一行第三列
    strcpy(img,line[2]);
    int img_fd = open(img,O_RDONLY);
    int read_size = 0;
    char ch;
    while((read_size = read(img_fd,&ch,1)) != 0)
    {
        write(1,&ch,1);
    }
    close(img_fd);
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
    char* name;
    strtok(query_string,"=");
    name = query_string + 5;
    SelectData(name);
    return 0;
}
