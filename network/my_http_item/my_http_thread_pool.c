#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <assert.h>
#include "thread_pool.h"

#define MAXEVENT 10
#define MAX 1024
#define MAX_METHOD 16
#define HOME_PAGE "index.html"
//打印用户手册
static void Usage()
{
    printf("Usage:./my_http [port]\n");
    exit(0);
}

//创建监听套接字
int StartUp(int port)
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }
    //强制征用正在TIME_WAIT状态的套接字
    int opt = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr =htonl(INADDR_ANY);
    server.sin_port = htons(port);
    
    //绑定端口号
    int ret = bind(sock,(const struct sockaddr*)&server,sizeof(server));
    if(ret < 0)
    {
        perror("bind");
        exit(2);
    }
    //监听
    ret = listen(sock,5);
    if(ret < 0)
    {
        perror("listen");
        exit(3);
    }
    return sock;
}

//逐行获取请求报文
int Getline(int client_sock, char buf[], int size)
{
    char ch = 'a';
    int i = 0;
    ssize_t s = 0;
    while(ch != '\n' && i < size-1 )
    {
        s = recv(client_sock,&ch,1,0);
        if(s > 0){
            if(ch == '\r')                        
            { 
                if(recv(client_sock,&ch,1,MSG_PEEK) > 0)
                {
                    if(ch == '\n')
                        recv(client_sock,&ch,1,0);           
                }
                ch = '\n';
            }
            buf[i++] = ch;                           
        }else
        {
            break;
        }
    }
    buf[i] = '\0';
    return i;
}

//将请求报头读完
void ClearHeader(int sock)
{
    char line[MAX];
    do{
        if(Getline(sock,line,MAX) <= 0)
            break;
        printf("%s",line);
    }while(strcmp(line,"\n") != 0);
}
//响应数据回去
void EchoWWW(int sock, char* path, int size, int *err)
{
    ClearHeader(sock);
    int fd = open(path,O_RDONLY);
    if(fd < 0)
    {
        *err = 404;
        return;
    }

    //发送响应报文
    char line[MAX];
    //发送状态行
    sprintf(line,"HTTP/1.0 200 OK\r\n");
    send(sock,line,strlen(line),0);
    //发送响应报头
    sprintf(line,"Content-Type:text/html;charset=ISO-8859-1\r\n");
    send(sock,line,strlen(line),0);
    //发送空行
    sprintf(line,"\r\n");
    send(sock,line,strlen(line),0);
    //将用户请求的资源响应回去
    sendfile(sock,fd,NULL,size);
    close(fd);
}

//404错误
void Error404(int sock)
{
    ClearHeader(sock);
    char err_buf[MAX] = "resources/404.html";
    int fd = open(err_buf,O_RDONLY);
    
    struct stat st;
    fstat(fd,&st);

    //发送响应报文
    char line[MAX];

    //发送状态行
    sprintf(line,"HTTP/1.0 200 OK\r\n");
    send(sock,line,strlen(line),0);

    //发送响应报头
    sprintf(line,"Content-Type:text/html;charset=ISO-8859-1\r\n");
    send(sock,line,strlen(line),0);
    //发送空行
    sprintf(line,"\r\n");
    send(sock,line,strlen(line),0);
    //将用户请求的资源响应回去
    sendfile(sock,fd,NULL,st.st_size);
    close(fd);
}

//发送错误
void EchoError(int sock,int code)
{
    switch(code)
    {
    case 404:
        Error404(sock);
        break;
    case 501:
        break;
    default:
        break;
    }
}

//CGI机制
void ExeCgi(int sock, char* method, char* query_string, char* path,int* errCode)
{
    char line[MAX];
    int content_lenth = -1;
    char method_env[MAX_METHOD];
    char query_string_env[MAX];
    char content_length_env[MAX/32];

    //cgi标准机制
    if(strcasecmp(method,"GET") == 0)
    {
        ClearHeader(sock);
    }
    else{
        do{
            Getline(sock,line,sizeof(line));
            printf("%s",line);
            if(strncmp(line,"Content-Length: ",16) == 0)
            {
                content_lenth = atoi(line+16);
            }
        }while(strcmp(line,"\n") != 0);
        if(content_lenth == -1)
        {
            *errCode = 404;
            return;
        }
    }


    //发送响应报文 
    //发送状态行 
    sprintf(line,"HTTP/1.0 200 OK\r\n"); 
    send(sock,line,strlen(line),0); 
    //发送响应报头 
    sprintf(line,"Content-Type: text/html;\r\n"); 
    send(sock,line,strlen(line),0); 
    //发送空行 
    sprintf(line,"\r\n"); 
    send(sock,line,strlen(line),0); 

    //用户提交数据，需要创建子进程执行程序(程序替换)并返回结果 
    //创建管道，用于父子进程通信 
    int input[2]; 
    int output[2]; 
    pipe(input); 
    pipe(output); 
    pid_t pid = fork(); 
    if(pid < 0)
    {
        *errCode = 404;
        return;
    }else if(pid == 0)
    {//child
        close(input[1]);
        close(output[0]);
        //重定向管道，为了读写方便
        dup2(input[0],0);
        dup2(output[1],1);

        //将参数以及资源路径导成环境变量
        sprintf(method_env,"METHOD=%s",method);
        putenv(method_env);
        if(strcasecmp(method,"GET") == 0)
        {
            sprintf(query_string_env,"QUERY_STRING=%s",query_string);
            putenv(query_string_env);
        }else if(strcasecmp(method,"POST") == 0){
            sprintf(content_length_env,"CONTENT_LENGTH=%d",content_lenth);
            putenv(content_length_env);
        }else
        {
            *errCode = 404;
            return;
        }
        //execl 程序替换
        //父进程交给子进程的数据 method ,GET[query_string] | POST[content_lenth]  
        execl(path,path,NULL);
        exit(1);
    }else{
        close(input[0]);
        close(output[1]);

        //如果方法是POST，父进程将参数写给子进程
        char ch;
        if(strcasecmp(method, "POST") == 0)
        {
            int i = 0;
            for(;i < content_lenth;i++)
            {
                read(sock,&ch,1);
                write(input[1],&ch,1);
            }
        }

        //父进程从管道中读出子进程执行结果，并响应给用户
        while(read(output[0],&ch,1) > 0)
        {
            send(sock,&ch,1,0);
        }
        waitpid(pid,NULL,0);
        close(input[1]);
        close(output[0]);
    }
    *errCode = 200;
}

//处理请求
static void* DealRequest(void* arg)
{
    int client_sock = *(int*)arg ;
    int errCode = 200;//响应给客户的错误码
    char method[MAX_METHOD];//获取请求方法
    char url[MAX];//URL
    char path[MAX];//资源路径
    int cgi = 0;//通用网关接口(用户向服务器提交数据时使用)
    char line[MAX];//用一个足够大的缓冲区直接将数据读走
    char* query_string = NULL;

#ifdef Debug
    //测试接收请求
    do{
        if(Getline(client_sock,line,MAX) <= 0)
            break;
        printf("%s",line);
    }while(strcmp(line,"\n") != 0);
#else
    if(Getline(client_sock,line,MAX) == 0)
    {
        //最好将错误信息打印到日志中，这里不作处理
        errCode = 404;
        goto end;
    }
    printf("%s",line);
    //获取请求方法
    int i = 0;
    int j = 0;
    while(i < sizeof(method)-1 && j < strlen(line) && !isspace(line[i]))
    {
        method[i++] = line[j++];
    }
    method[i] = '\0';
    //目前只负责处理GET与POST方法，如果两个都不是，则直接响应错误信息
    if(strcasecmp(method,"GET") && strcasecmp(method,"POST"))
    {
        errCode = 404;
        goto end;
    }

    //获取URL
    while(j < strlen(line) && isspace(line[j]))
        j++;
    i = 0;
    while(i < sizeof(url)-1 && j < strlen(line) && !isspace(line[j]))
    {
        url[i++] = line[j++];
    }
    url[i] = '\0';
    //解析方法
    if(strcasecmp(method,"GET") == 0)
    {
        query_string = url;
        while(*query_string)
        {
            if(*query_string == '?')
            {
                *query_string = '\0';
                query_string++;
                cgi = 1;
                break;
            }
            else
            {
                query_string++;
            }
        }
    }

    //URL中参数前的是资源路径，而我们的资源放置在当前目录下的./resources中
    //所以要将两部分拼接起来
    //url -> ./resources/a/b/c.html
    sprintf(path,"resources%s",url);

    //如果url只有一个'\',就需要把首页响应回去
    if(path[strlen(path)-1] == '/')
    {
        strcat(path,HOME_PAGE);
    }

    //判断path中要请求的资源是否存在,即是否合法
    struct stat st;
    if(stat(path,&st) < 0)//文件不存在
    {
        errCode = 404;
        goto end;
    }else
    {
        //访问的可能是一个目录
        //把首页响应回去
        if(S_ISDIR(st.st_mode)){
            strcat(path,HOME_PAGE);
        }else{
            //请求访问的文件可能有可执行权限
            //如果有，需要以cgi方式运行
            if((st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH) || (st.st_mode & S_IXUSR))
            {
                cgi = 1;
            }
        }
        if(cgi){
            ExeCgi(client_sock,method,query_string,path,&errCode);
        }
        else
        {
            //到这里表明请求肯定是不带参数的GET方法
            //发送响应报文
           EchoWWW(client_sock,path,st.st_size,&errCode); 
        }
    }

#endif


end:
    //http是请求响应，响应一次就关闭连接
    if(errCode != 200){
        EchoError(client_sock,errCode);
    }
    close(client_sock);
    return NULL;
}


int main(int argc, char* argv[])
{
    if(argc != 2)
        Usage();
    //创建服务端TCP套接字
    int listen_sock = StartUp(atoi(argv[1]));
    //忽略SIGPIPE信号
    signal(SIGPIPE,SIG_IGN);
    PoolInit(5);
    for(;;){
        struct sockaddr_in client;
        socklen_t len;
        int client_sock = accept(listen_sock,(struct sockaddr*)&client,&len);
        if(client_sock < 0)
        {
            continue;
        }

        //int *_sock = (int*)malloc(sizeof(int));
        //*_sock = client_sock;
        PoolAddWorker(DealRequest,&client_sock);
        //free(_sock);
    }

    PoolDestroy();

    return 0;
}
