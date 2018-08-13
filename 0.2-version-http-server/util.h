////////////////////////////////////////////////////
// 代码大全(code complete)
// 软件工程本质上是对复杂度的管理
// 此文件放置一些工具类和函数，
// 为了让这些工具用的方便，直接把声明和实现都放在.hpp中
// /////////////////////////////////////////////////
#pragma once
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 1024 //缓冲区大小
enum LogLevel{
    INFO,//信息
    WARNING,//警告
    ERROR, //错误
    CRITIAL//致命缺陷
}LogLevel;

//逐行获取请求报文
//为了统一格式 将所有的行末的"\r\n"或'\r' 都替换成 '\n' 
int GetLine(int sock, char buf[], int size);

//将请求报头剩余部分一次性读完
void ClearHeader(int sock);

//读取配置文件
int ReadEtc();


//打印日志
//获取到当前的秒级时间戳
static int64_t TimeStamp();
//获取到当前的微秒级时间戳
static int64_t TimeStampUS();

// __FILE__ __LINE__
#define LOG(level) Log(level,__FILE__, __LINE__)


// LOG(INFO) << "HELLO"
// [I时间戳 util.hpp:31] HELLO
void Log(enum LogLevel level, char* file, int line);
//信号处理函数
void HandleForSigpipe();

//将socket设置为非阻塞模式
int SetSocketNoBlock(int sock);



