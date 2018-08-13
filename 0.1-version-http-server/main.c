#include "server.h"
#include "thread_pool.h"
#include "myepoll.h"
#include "util.h"
#include "timer.h"
#define MAXEVENTS 50
int main()
{
    HandleForSigpipe();
    int port = ReadEtc();
    int listen_sock = StartUp(port);
    //初始化线程池
    PoolInit(THREAD_NUM);
    //初始化Epoll
    int epoll_fd = EpollInit();
    //初始化定时器堆
    //TimerHeapInit();

    epoll_event* events = (epoll_event*)malloc(sizeof(epoll_event) * MAXEVENTS);
    if(events == NULL)
    {
        perror("malloc");
        return 0;
    }
    EpollAdd(epoll_fd,listen_sock,EPOLLIN,0);
    //PoolAddWorker(ListenTimerHeap,(void*)&epoll_fd);
    for(;;)
    {
        int ret = EpollWait(epoll_fd,MAXEVENTS,-1,events);
        if(ret <= 0)
        {
            continue;
        }
        
        int i = 0;
        for(; i < ret; i++)
        {
            if(events[i].data.fd == listen_sock)
            {
                Accept(epoll_fd, listen_sock);
            }
            else if(events[i].events & EPOLLHUP)
            {
                continue;
            }
            else
            {
                //Timer tm;
                //CreateTimer(&tm,events[i].data.fd);
                //TimerAdd(tm);
                CommonArg* client_arg = (CommonArg*)malloc(sizeof(CommonArg));
                client_arg->sock = events[i].data.fd;
                client_arg->epoll_fd = epoll_fd;
                //client_arg->tm = tm;
                PoolAddWorker(Request,(void*)client_arg);
            }
        }
    }
    //TimerHeapDestroy();
    free(events);
    return 0;
}
