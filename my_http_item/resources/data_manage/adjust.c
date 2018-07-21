#include <stdio.h>
#include "/home/wh/31class/network/my_http_item/lib/include/mysql.h"
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#define MAX 1024


void AdjustData(char* phone, int money)
{
    //初始化
    MYSQL *my_sql = NULL;
    my_sql = mysql_init(NULL);
    
    printf("<html>");
    printf("<body>");
    if(strlen(phone) != 11)
    {
        printf("<h2>电话号码填写不规范,请返回进行修改</h2>");
        printf("<form action=\"./adjust\" method=\"GET\">");
        printf("手机号码:<br>");
        printf("<input type=\"text\" name=\"phone\" value=\"\">");
        printf("<br>");       
        printf("余额:<br>");
        printf("<input type=\"text\" name=\"money\" value=\"\">");
        printf("<br>");
        printf("<br><br>");
        printf("<input type=\"submit\" value=\"确认再次修改\">");
        printf("<br>");
        printf("</form>");
        printf("</body>");
        printf("</html>");
        return;
    }else if(money < 0)
    {
        printf("<h2>余额输入错误,请返回进行修改</h2>");
        printf("<form action=\"./adgust\" method=\"GET\">");
        printf("手机号码:<br>");
        printf("<input type=\"text\" name=\"phone\" value=\"\">");
        printf("<br>");       
        printf("余额:<br>");
        printf("<input type=\"text\" name=\"money\" value=\"\">");
        printf("<br>");
        printf("<br><br>");
        printf("<input type=\"submit\" value=\"确认再次修改\">");
        printf("<br>");
        printf("</form>");
        printf("</body>");
        printf("</html>");
        return;
    }
    //连接数据库
    if(mysql_real_connect(my_sql, "127.0.0.1", "root", "", "myhttp", 3306, NULL, 0) ){
        printf( "<h3>CONNECT MYSQL SUCCESS</h3>");
    }else{
        printf( "<h3>FAIL</h3>"  );
        return ;                     
    }

    //执行SQL语句
    char words[MAX];
    sprintf(words,"update user set money = %d where phone = %s",money,phone);
    if(mysql_query(my_sql,words) != 0)
    {
        printf("<h4>INSERT FAIL</h4>");
    }
    
    sprintf(words,"select * from user where phone =%s",phone);
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
        printf("<h2>并未查找到此会员信息,请查看手机号码是否填写正确<h2>");
        printf("<a href=\"../index.html\">返回查询界面进行修改</a>");
        printf("</html>");
        printf("</body>");
        return;
    }
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
    printf("<body background=\"../img/raise.jpg\">");
    printf("</table>");
    printf("</body>");
    printf("</html>");
    free(result);
    mysql_close(my_sql);
    //关闭数据库连接
    
}

int main(void)
{
    char method[MAX/32];
    strcpy(method,getenv("METHOD"));
    int content_length = 0;
    char query_string[MAX];
    if(strcasecmp(method,"GET") == 0)
    {
        strcpy(query_string,getenv("QUERY_STRING"));
    }else if(strcasecmp(method,"POST") == 0)
    {
        content_length = atoi(getenv("CONTENT_LENGTH"));
        int i = 0;
        for(; i < content_length; ++i)
        {
            read(0,query_string+i,1);
        }
    }
    //phone=&money=
    char *phone;
    int money;
    strtok(query_string,"=&");
    phone = strtok(NULL,"=&");
    strtok(NULL,"=&");
    money = atoi(strtok(NULL,"=&"));

    AdjustData(phone,money);
    return 0;
}



