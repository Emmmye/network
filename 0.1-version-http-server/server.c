#include "server.h"
#include "myepoll.h"
#include "util.h"
// 启动CGI机制位
int cgi = 0;
//状态码
int buffer_code = 0;


int StartUp(int port)//创建监听socket
{
    int listen_sock = socket(AF_INET,SOCK_STREAM,0);
    if(listen_sock == -1)
    {
        perror("socket");
        exit(1);
    }

    //强行征用正在TIME_WAIT状态的套接字
    int opt = 1;
    setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    //预防DDOS攻击,清除半连接态
    opt = 3;
    setsockopt(listen_sock,SOL_TCP,TCP_DEFER_ACCEPT,&opt,sizeof(opt));


    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    int ret = bind(listen_sock,(sockaddr*)&server,sizeof(server));
    if(ret < 0)
    {
        perror("bind");
        exit(1);
    }

    ret = listen(listen_sock,10);
    if(ret == -1)
    {
        perror("listen");
        exit(1);
    }

    return listen_sock;
}

int Accept(int epoll_fd,int sock) //接收连接
{
    sockaddr_in client;
    socklen_t len;
    int client_sock = accept(sock,(sockaddr*)&client,&len);
    if(client_sock < 0)
    {
        perror("accept");
        return -1;
    }

    EpollAdd(epoll_fd,client_sock,EPOLLIN,1);
    return 0;
}

//获得请求方法
int GetMethod(char* line, char* method)
{
    size_t i = 0 , j = 0;
    while(i < 10 && j < strlen(line) && !isspace(line[j]))
    {
        method[i++] = line[j++];
    }
    method[i] = '\0';
    //此时line[j] 为空格
    //j+1 刚好指向 URL的起始位置
    return j+1;
}

//获得URL
int GetUrl(char* line, char* url, int start)
{
    size_t i = 0;
    while(i < MAX_LINE-1 && line[start] != '\n' && !isspace(line[start]))
    {
        url[i++] = line[start++];
    }
    url[i] = '\0';
    //如果有参数，此时 i 指向 ?,
    //如果没有,此时指向空格
    return i;
}

void* Request(void* arg) //处理请求
{
    CommonArg* cur = (CommonArg*)arg;
    int client_sock = cur->sock;
    int epoll_fd = cur->epoll_fd;
    //TimeRemove(&(cur->tm), epoll_fd);
    //获取请求报头 header 行，并进行分析
    char line[MAX_LINE] = {0};
    size_t read_size = 0;
    read_size = GetLine(client_sock,line,MAX_LINE);
    if(read_size == 0)
    {
        buffer_code = 404;
        goto end;
        EchoErrno(client_sock);
    }

    //获得请求方法
    char method[10] = {0};
    int start = GetMethod(line,method);

    //获得 URL
    char url[MAX_LINE] = {0};
    start = GetUrl(line,url,start);

    //获得资源路径,以及GET方法中的参数
    char* query_string = url;
    if(strcasecmp(method,"GET") == 0){
        while(*query_string )    
        {
            if(*query_string == '?')
            {
                *query_string = '\0';
                query_string++;
                cgi = 1;
                break;
            }else
            {
                query_string++;
            }
        }
    }

    //URL中参数前的是请求的资源路径，而我们的资源放在当前目录下的./wwwroot中
    //所以要将两部分拼接起来
    //url -> ./wwwroot/index.html
    char path[MAX_LINE] = {0};
    sprintf(path,"wwwroot%s",url);

    if(path[strlen(path)-1] == '/')
    {
        strcat(path,HOMEPAGE);
    }

    //判断要请求的资源是否存在,即是否合法
    struct stat st;
    if(stat(path,&st) < 0)
    {
        buffer_code = 404;
        EchoErrno(client_sock);
        goto end;
    }else
    {
        if(S_ISDIR(st.st_mode))
        {
            strcat(path,HOMEPAGE);
        }else
        {
            //请求访问的文件可能有可执行权限
            //如果有，就以CGI方式运行
            if((st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH) || (st.st_mode & S_IXUSR))
            {
                cgi = 1;
            }
        }
        if(cgi)
        {
            ReponseByCgi(client_sock,method, query_string,path);
        }else
        {
            ReponseNormal(client_sock, path,st.st_size);
        }
    }

end:
    if(buffer_code != 200){
        EchoErrno(client_sock);
    }
    EpollDel(epoll_fd,client_sock,EPOLLIN | EPOLLONESHOT);
    free(cur);
    close(client_sock);
    return NULL;

}

void ReponseByCgi(int client_sock,char* method,char* query_string, char* path)
{
    char line[MAX_LINE];
    int content_lenth = -1;
    char method_env[MAX_LINE / 16];
    char query_string_env[MAX_LINE];
    char content_length_env[MAX_LINE/32];

    //cgi标准机制
    if(strcasecmp(method,"GET") == 0)
    {
        ClearHeader(client_sock);
    }
    else{
        do{
            GetLine(client_sock,line,sizeof(line));
            if(strncmp(line,"Content-Length: ",16) == 0)
            {
                content_lenth = atoi(line+16);
            }
        }while(strcmp(line,"\n") != 0);
        if(content_lenth == -1)
        {
            buffer_code = 404;
            return;
        }
    }



    //用户提交数据，需要创建子进程执行程序(程序替换)并返回结果 
    //创建管道，用于父子进程通信 
    int input[2]; 
    int output[2]; 
    pipe(input); 
    pipe(output); 
    pid_t pid = fork(); 
    if(pid < 0)
    {
        printf("fork error\n");
        buffer_code = 404;
        return;
    }else if(pid == 0)
    {//child
        close(input[1]);
        close(output[0]);
        //重定向管道，为了读写方便
        dup2(input[0],0);
        dup2(output[1],1);

        //将参数以及资源路径导成环境变量
        char sock_env[MAX_LINE/10];
        sprintf(sock_env,"SOCK=%d",client_sock);
        putenv(sock_env);
        sprintf(method_env,"METHOD=%s",method);
        putenv(method_env);
        if(strcasecmp(method,"GET") == 0)
        {
            sprintf(query_string_env,"QUERY_STRING=%s",query_string);
            putenv(query_string_env);
        }else if(strcasecmp(method,"POST") == 0 ){
            sprintf(content_length_env,"CONTENT_LENGTH=%d",content_lenth);
            putenv(content_length_env);
        }
        else
        {
            buffer_code = 404;
            return;
        }
        //execl 程序替换
        //父进程交给子进程的数据 method ,GET[query_string] | POST[content_lenth]  
        execl(path,path,NULL);
        exit(1);
    }else{
        close(input[0]);
        close(output[1]);
        char ch;
        //如果方法是POST，父进程将参数写给子进程
        if(strcasecmp(method,"POST") == 0 )
        {
            int i = 0;
            for(;i < content_lenth;i++)
            {
                read(client_sock,&ch,1);
                write(input[1],&ch,1);
                
            }
        }

        //父进程从管道中读出子进程执行结果，并响应给用户
        int s = 0;
        char read_buf[MAX_LINE * 100];
        int read_index = 0;
        while(read(output[0],read_buf+read_index,1) > 0)
        {
            s++;
            read_index++;
        }
        //发送响应报文 
        //发送状态行 
        sprintf(line,"HTTP/1.0 200 OK\r\n"); 
        send(client_sock,line,strlen(line),0); 
        //发送响应报头 
        if(read_index > 10000)
        {
            sprintf(line,"Content-Type:image/png;\r\n");
            send(client_sock,line,strlen(line),0);
        }
        else{
            sprintf(line,"Content-Type:text/html;charset=utf-8;\r\n");
            send(client_sock,line,strlen(line),0);
        }
        //发送空行 
        sprintf(line,"\r\n"); 
        send(client_sock,line,strlen(line),0);
        int write_index = 0;
        while(write_index < read_index)
        {
            send(client_sock,read_buf+write_index,1,0);
            write_index++;
        }
        waitpid(pid,NULL,0);
        close(input[1]);
        close(output[0]);
    }
    buffer_code = 200;

}
void ReponseNormal(int client_sock,char* path, size_t size)
{
    ClearHeader(client_sock);
    int fd = open(path,O_RDONLY);
    if(fd < 0)
    {
        buffer_code = 404;
        return;
    }

    //发送响应报文
    char line[MAX_LINE];
    //发送状态行
    sprintf(line,"HTTP/1.0 200 OK\r\n");
    send(client_sock,line,strlen(line),0);
    //发送响应报头
    sprintf(line,"Content-Type:text/html;charset=utf-8\r\n");
    send(client_sock,line,strlen(line),0);
    //发送空行
    sprintf(line,"\r\n");
    send(client_sock,line,strlen(line),0);
    //将用户请求的资源响应回去
    sendfile(client_sock,fd,NULL,size);
    buffer_code = 200;
    close(fd);

}
//响应错误
void EchoErrno(int sock)
{
    switch(buffer_code)
    {
    case 404:
        EchoErrno404(sock);
        break;
    case 501:
        break;
    default:
        break;
    }
}

//响应404
void EchoErrno404(int client_sock)
{
    ClearHeader(client_sock);
    char err_buf[MAX_LINE] = "wwwroot/404.html";
    int fd = open(err_buf,O_RDONLY);
    
    struct stat st;
    fstat(fd,&st);

    //发送响应报文
    char line[MAX_LINE];

    //发送状态行
    sprintf(line,"HTTP/1.0 200 OK\r\n");
    send(client_sock,line,strlen(line),0);

    //发送响应报头
    sprintf(line,"Content-Type:text/html;charset=utf-8\r\n");
    send(client_sock,line,strlen(line),0);
    //发送空行
    sprintf(line,"\r\n");
    send(client_sock,line,strlen(line),0);
    //将用户请求的资源响应回去
    sendfile(client_sock,fd,NULL,st.st_size);
    close(fd);

}
