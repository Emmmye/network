#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include "/home/wh/31class/network/my_http_item/lib/include/mysql.h"

#define MAX 1024

void InsertData(char* name, char* phone , int money)
{
    //初始化
    MYSQL *my_sql = NULL;
    my_sql = mysql_init(NULL);
    
    printf("<html>");
    printf("<body>");
    if(strlen(phone) != 11)
    {
        printf("<h2>添加新用户失败,电话号码填写不规范</h2>");
        printf("<a href=\"../regist.html\">返回进行修改</a>");
        printf("</html>");
        printf("</body>");
        return;
    }else if(money < 100)
    {
        printf("<h2>初次办理会员必须超过100元</h2>");
        printf("<a href=\"../regist.html\">返回进行修改</a>");
        printf("</html>");
        printf("</body>");
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
    sprintf(words,"insert into user values(\"%s\",\"%s\",\"%d\")",name,phone,money);
    printf("<h4>%s</h4>",words);
    if(mysql_query(my_sql,words) != 0)
    {
        printf("<h4>INSERT FAIL</h4>");
    }
    
    //关闭数据库连接
    mysql_close(my_sql);
    printf("<body background=\"../img/city.jpg\">");

    printf("</body>");
    printf("</html>");
}
int main()
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
    //name=&sex=&phone=
    //提取字段
    char *name,*phone;
    int money;
    strtok(query_string,"=&");
    name = strtok(NULL,"=&");
    strtok(NULL,"=&");
    phone = strtok(NULL,"=&");
    strtok(NULL,"=&");
    money = atoi(strtok(NULL,"=&"));
    InsertData(name,phone,money);
    return 0;
}
