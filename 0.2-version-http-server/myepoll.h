#pragma once

#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct epoll_event epoll_event;


int EpollInit(); //初始化epoll句柄
int EpollAdd(int epoll_fd ,int fd,__uint32_t events, int oneshot); //向epoll中注册事件
int EpollDel(int epoll_fd ,int fd,__uint32_t events); //从epoll中删除fd
int EpollMod(int epoll_fd, int fd,__uint32_t events); //修改事件
int EpollWait(int epoll_fd,int max_events,int timeout,epoll_event* events); // 从epoll中获取就绪事件
void EpollDestroy(int epoll_fd); //关闭epoll_fd

