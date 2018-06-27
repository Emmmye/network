#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#define MAX 1024

void MyCal(int x , int y)
{
    printf("<html>\n");
    printf("<body>\n");
    printf("<h3>%d + %d = %d </h3>\r\n",x,y,x+y);
    printf("<h3>%d - %d = %d </h3>\r\n",x,y,x-y);
    printf("<h3>%d * %d = %d </h3>\r\n",x,y,x*y);
    if(y == 0){
        printf("<h3>%d / %d = %d </h3>\r\n",x,y,-1);
        printf("<h3>%d %% %d = %d </h3>\r\n",x,y,-1);
    }else
    {
        printf("<h3>%d / %d = %d </h3>\r\n",x,y,x/y);
        printf("<h3>%d %% %d = %d </h3>\r\n",x,y,x%y);
    }
    printf("</body>\r\n");
    printf("</html>\r\n");
}


int main()
{
    char method[MAX/32] = {0};
    strcpy(method,getenv("METHOD"));
    char query_string[MAX];
    int x , y;
    if(strcasecmp(method,"GET") == 0)
    {
        strcpy(query_string,getenv("QUERY_STRING"));
        sscanf(query_string,"firstname=%d&lastname=%d",&x,&y);
        MyCal(x,y);
    }else{
        int content_length = -1;
        char content_length_env[MAX/32];
        strcpy(content_length_env,getenv("CONTENT_LENGTH"));
        content_length = atoi(content_length_env);
        int i = 0;
        char ch;
        while(i < content_length)
        {
          read(0,&ch,1);
          query_string[i++] = ch;
        }
        query_string[i] = '\0';
        sscanf(query_string,"firstname=%d&lastname=%d",&x,&y);
        MyCal(x,y);
    }
}
