#pragma once

#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/epoll.h>

#define MAX_TIMER 2000


typedef struct Timer{
    long long time_start;
    int sock;
}Timer;


typedef struct TimerHeap{
    Timer TimerData[MAX_TIMER];
    size_t size;
    pthread_mutex_t del_lock;
}TimerHeap;

//创建定时器
void CreateTimer(Timer* new_tm, int sock);

//初始化定时器堆
void TimerHeapInit();

//销毁定时器堆
void TimerHeapDestroy();

//添加定时器
int TimerAdd(Timer tm);

//删除堆顶元素
int TimerDel(int epoll_fd);

//删除特定的结构体
int TimeRemove(Timer* tm, int epoll_fd);

//判断堆是否为空
int IsEmpty();

//求此时定时器的个数
size_t Size();

//轮询定时器堆
void* ListenTimerHeap(void* arg);



