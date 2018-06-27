#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <pthread.h>  
#include <assert.h>  
//任务队列中的节点
typedef struct worker{
    //回调函数
    //任务运行时会调用该函数，也就是空闲线程会执行该函数
    void *(*Request) (void* arg);
    //回调函数参数
    void *arg;
    struct worker *next;
}Cthread_worker;


//线程池结构
typedef struct{
    pthread_mutex_t queue_lock; /*互斥锁*/
    pthread_cond_t queue_ready; /*有新任务到来时，唤醒阻塞等待的空闲线程*/
    
    pthread_t *threadid;
    Cthread_worker *head;       /*任务队列的头指针*/
    int queue_size;             /*当前等待队列中的任务数目*/
    int max_thread_num;         /*线程池中允许的最大线程数目*/
    int shutdown;               /*是否销毁线程池*/
}Cthread_pool;


void PoolInit(int max);         /*初始化线程池*/

int PoolDestroy();             /*销毁线程池*/

int PoolAddWorker(void *(*Request)(void* arg), void* arg); /*往线程池中添加一个任务*/

void* PoolRoutine(void *arg);    /*线程刚创建时执行的函数*/
