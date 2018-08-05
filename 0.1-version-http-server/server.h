#pragma once

#define THREAD_NUM 5 //线程池初始个数
#define HOMEPAGE "index.html" //首页资源名称
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct CommonArg{
    int epoll_fd;
    int sock;
}CommonArg;


#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
#include <ctype.h>


int StartUp(int port); //创建监听socket
int Accept(int epoll_fd, const int sock); //接收连接

void* Request(void* arg); //处理请求
//响应动态页面
void ReponseByCgi(int client_sock, char* method, char* query_string,char* path);
//响应静态页面
void ReponseNormal(int client_sock, char* path,  size_t size);
//响应错误信息
void EchoErrno(int client_sock);
//响应404
void EchoErrno404(int client_sock);




