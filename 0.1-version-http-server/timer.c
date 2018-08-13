#include "timer.h"


TimerHeap* th = NULL;
//创建定时器
void CreateTimer(Timer* new_tm, int sock)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    new_tm->time_start = tv.tv_usec*1000;
    new_tm->sock = sock;
    return;
}

// typedef struct Timer{
//     struct timeval tv_start;
//     int sock;
// }Timer;
// 
// 
// typedef struct TimerHeap{
//     Timer TimerData[MAX_TIMER];
//     size_t size;
// }TimerHeap;
// 
// TimerHeap* th = NULL;

//初始化定时器堆
void TimerHeapInit()
{
    th = (TimerHeap*)malloc(sizeof(TimerHeap));
    if(th == NULL)
    {
        perror("malloc");
        exit(1);
    }
    
    th->size = 0;
    pthread_mutex_init(&(th->del_lock),NULL);
    memset(th->TimerData,0,sizeof(th->TimerData));
    return;
}

//销毁定时器堆
void TimerHeapDestroy()
{
    if(th)
    {
        pthread_mutex_destroy(&(th->del_lock));
        free(th);
    }
    return;
}

//交换函数
void Swap(Timer* tm1, Timer* tm2)
{
    Timer tmp = *tm1;
    *tm1 = *tm2;
    *tm2 = tmp;
}


//上浮函数
void AdjustUp(Timer* _tm, size_t child)
{
    if(_tm == NULL)
        return;

    if(child == 0)
    {
        return;
    }
    
    size_t parents = (child - 1) / 2;
    if(_tm[child].time_start < _tm[parents].time_start)
    {
        Swap(&(_tm[child]),&(_tm[parents]));
        AdjustUp(_tm,parents);
    }

    return;
}

//下沉函数
void AdjustDown(Timer* _tm, size_t parents)
{
    if(_tm == NULL)
        return;

    if(parents >= th->size-1)
    {
        return;
    }

    size_t child = 2 * parents + 1;
    while(child > 0 && child < th->size)
    {
        if(child + 1 < th->size && _tm[child].time_start < _tm[child+1].time_start)
        {
            child++;
        }

        if(_tm[parents].time_start > _tm[child].time_start)
        {
            Swap(&(_tm[parents]), &(_tm[child]));
            parents = child;
            child = 2 * parents + 1;
        }else
        {
            break;
        }
    }

    return;
}



//添加定时器
int TimerAdd(Timer tm)
{
    if(th->size == MAX_TIMER)
    {
        return -1;
    }
    if(th->size == 0)
    {
        th->TimerData[th->size++] = tm;
        return 1;
    }

    th->TimerData[th->size] = tm;
    AdjustUp(th->TimerData,th->size);
    th->size++;
    return 1;
}

//删除堆顶元素
int TimerDel(int epoll_fd)
{
    if(th->size == 0)
    {
        return -1;
    }

    if(th->size == 1)
    {
        th->size--;
        return 1;
    }

    if(th->TimerData[0].time_start != 0){
        close(th->TimerData[0].sock);
    }
    struct epoll_event event;
    event.data.fd = th->TimerData[0].sock;
    event.events = EPOLLIN | EPOLLONESHOT;
    epoll_ctl(epoll_fd,EPOLL_CTL_DEL,th->TimerData[0].sock,&event);
    Swap(&th->TimerData[0],&th->TimerData[th->size-1]);
    th->size--;
    AdjustDown(th->TimerData,0);
    return 1;
}

//删除特定的结构体
int TimeRemove(Timer* tm,int epoll_fd)
{
    if(tm == NULL)
        return -1;
    if(th->size == 0)
        return 1;
    pthread_mutex_lock(&(th->del_lock));
    size_t i = 0;
    printf("申请th->del_lock成功\n");
    while(i < th->size && th->TimerData[i].sock != (*tm).sock)
    {
        i++;
    }
    if(i == th->size)
        return -1;
    th->TimerData[i].time_start = 0;
    AdjustUp(th->TimerData,i);
    TimerDel(epoll_fd);
    pthread_mutex_unlock(&(th->del_lock));
    printf("释放th->del_lock成功\n");
    return 1;
}

//判断堆是否为空
int IsEmpty()
{
    if(th->size != 0)
        return 0;
    return 1;
}


//求此时定时器的个数
size_t Size()
{
    return th->size;
}

//轮询定时器堆
void* ListenTimerHeap(void* arg)
{
    int epoll_fd = *(int*)(arg);
    struct timeval tv;
    while(1){
        usleep(500);
        gettimeofday(&tv,NULL);
        if(th->size == 0)
            continue;
        pthread_mutex_lock(&(th->del_lock));
        while(th->TimerData[0].time_start + 500 >= tv.tv_usec * 1000)
        {
            TimerDel(epoll_fd);
        }
        pthread_mutex_unlock(&(th->del_lock));
    }
    return NULL;
}
