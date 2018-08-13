#include "myepoll.h"
const int MAXEVENTS = 5000;
const int LISTENQ = 1024;

//初始化epoll句柄   
int EpollInit() 
{
    int epoll_fd = epoll_create(LISTENQ);

    if(epoll_fd == -1)
    {
        perror("epoll_create");
        exit(1);
    }


    return epoll_fd;

}

 //向epoll中注册事件   
int EpollAdd(int epoll_fd ,int fd,__uint32_t events, int oneshot)
{
    epoll_event event;
    event.events = events;
    event.data.fd = fd;
    if(oneshot)
    {
        event.events |= EPOLLONESHOT;
    }
    int ret = epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&event);
    if(ret == -1)
    {
        perror("EpollAdd error");
        return -1;
    }
    
    return 0;
}

//修改事件     
int EpollMod(int epoll_fd, int fd,__uint32_t events) 
{
    epoll_event event;
    event.events = events;
    event.data.fd = fd;
    int ret = epoll_ctl(epoll_fd,EPOLL_CTL_MOD,fd,&event);
    if(ret == -1)
    {
        perror("EpollMod error");
        return -1;
    }
    
    return 0;

}

//从epoll中删除fd   
int EpollDel(int epoll_fd ,int fd,__uint32_t events) 
{
    epoll_event event;
    event.events = events;
    event.data.fd = fd;
    int ret = epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,&event);
    if(ret == -1)
    {
        perror("EpollDel error");
        return -1;
    }
    
    return 0;

}

// 从epoll中获取就绪事件    
int EpollWait(int epoll_fd,int max_events,int timeout,epoll_event* events) 
{
    int ret = epoll_wait(epoll_fd,events,max_events,timeout);
    if(ret < 0)
    {
        perror("EpollWait error");
        return -1;
    }

    return ret;
}
void EpollDestroy(int epoll_fd) //关闭epoll_fd
{
    close(epoll_fd);
}
