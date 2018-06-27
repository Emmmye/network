#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include "/home/wh/31class/network/my_http_item/lib/include/mysql.h"

#define MAX 1024
void SelectData()
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
    const char* words = "select * from student_info";
    if(mysql_query(my_sql,words))
    {
        printf("<h4>插入失败</h4>");
        return;
    }
    //接收查询结果
    //该函数开辟了一段内存用来保存查询结果,后面一定要记得释放内存
    MYSQL_RES *result = mysql_store_result(my_sql);
    //获取行数
    my_ulonglong rows = mysql_num_rows(result);
    //获取列数
    unsigned cols = mysql_num_fields(result);
    //获取列名
    MYSQL_FIELD *field = mysql_fetch_fields(result);
    printf("<table border=\"1\">");
    unsigned int i = 0;
    printf("<tr>");
    for(; i < cols; ++i)
    {
        printf("<td>%s</td>",field[i].name);
    }
    printf("</tr>");
    //获取列表内容
    MYSQL_ROW line;
    for(i = 0; i < rows ; ++i)
    {
        line =  mysql_fetch_row(result);
        unsigned int j = 0;
        printf("<tr>");
        for(; j < cols; ++j)
        {
            printf("<td>%s</td>",line[j]);
        }
        printf("</tr>");
    }
    printf("<body background=\"../img/autumn.jpg\">");
    printf("</table>");
    printf("</body>");
    printf("</html>");
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
    SelectData();
    return 0;
}
