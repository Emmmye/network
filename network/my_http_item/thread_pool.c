#include "thread_pool.h"


int PoolAddWorker (void *(*process) (void *arg), void *arg); 
void* PoolRoutine(void *arg);    /*线程刚创建时执行的函数*/

static Cthread_pool *pool = NULL;

//初始化线程池
void PoolInit(int max)
{
    pool = (Cthread_pool*)malloc(sizeof(Cthread_pool));
    if(pool == NULL)
        return;
    pthread_mutex_init(&(pool->queue_lock),NULL);
    pthread_cond_init(&(pool->queue_ready),NULL);

    pool->max_thread_num = max;
    pool->queue_size = 0;

    pool->shutdown = 0;

    pool->threadid = (pthread_t*)malloc(max*sizeof(pthread_t));
    if(pool->threadid == NULL)
        return;
    int i = 0;
    for(; i < max; ++i)
    {
        pthread_create(&(pool->threadid[i]),NULL,PoolRoutine,NULL);
    }
    pool->head = NULL;
}

int PoolDestroy()/*销毁线程池*/
{
    if(pool->shutdown)
        return -1;      /*防止两次调用*/

    pool->shutdown = 1;

    /*销毁线程池时,唤醒所有阻塞的进程*/
    pthread_cond_broadcast(&(pool->queue_ready));

    /*阻塞等待进程退出,否则就成僵尸了*/
    int i = 0;
    for(; i < pool->max_thread_num ; ++i)
    {
        pthread_join(pool->threadid[i],NULL);
    }
    free(pool->threadid);

    /*销毁等待队列*/
    Cthread_worker *to_delete = NULL;
    for(i = 0; i < pool->queue_size; ++i)
    {
        to_delete = pool->head;
        pool->head = pool->head->next;
        free(to_delete);
    }

    pthread_mutex_destroy(&(pool->queue_lock));
    pthread_cond_destroy(&(pool->queue_ready));

    /*销毁后指针为空*/
    free(pool);
    pool = NULL;
    return 0;
    
}

int PoolAddWorker(void *(*Request)(void* arg), void* arg) /*往线程池中添加一个任务*/
{
    /*构造新任务*/
    Cthread_worker *worker = (Cthread_worker*)malloc(sizeof(Cthread_worker));
    if(worker == NULL)
        return -1;

    worker->Request = Request;
    worker->arg = arg;
    worker->next = NULL;

    pthread_mutex_lock(&(pool->queue_lock));

    /*将任务加入到等待队列中*/
    Cthread_worker* new_node = pool->head;
    if(new_node != NULL)
    {
        while(new_node->next != NULL)
            new_node = new_node->next;
        new_node->next = worker;
    }else
    {
        pool->head = worker;
    }
    pool->queue_size += 1;

    assert(pool->head != NULL);

    pthread_mutex_unlock(&(pool->queue_lock));
    /*唤醒一个等待的线程*/
    /*如果所有线程都在忙碌的话,这句话是没用的*/
    pthread_cond_signal (&(pool->queue_ready));  
    return 0;
}

void* PoolRoutine(void *arg)    /*线程刚创建时执行的函数*/
{
    const char* p = (const char*)arg;
    printf("starting thread %lu:[%s]\n",pthread_self(),p);
    while(1)
    {
        /*如果等待队列为0，并且线程池不需要被销毁，则线程处于阻塞状态*/
        pthread_mutex_lock(&(pool->queue_lock)); 

        while (pool->queue_size == 0 && !pool->shutdown)  
        {  
            printf ("thread 0x%lu is waiting\n", pthread_self ());  
            pthread_cond_wait (&(pool->queue_ready), &(pool->queue_lock));  
        }  

        if(pool->shutdown)
        {
            /*遇到break,continue,return等跳转语句，千万不要忘记先解锁*/
            pthread_mutex_unlock(&(pool->queue_lock));
            printf ("thread 0x%lu will exit\n", pthread_self ());  
            pthread_exit (NULL);
        }

        printf ("thread 0x%lu is starting to work\n", pthread_self ());  

        assert(pool->queue_size != 0);
        assert(pool->head != NULL);

        /*取出等待队列的第一个任务,等待队列中任务数目减1*/
        pool->queue_size -= 1;
        Cthread_worker* worker = pool->head;
        pool->head = pool->head->next;
        pthread_mutex_unlock (&(pool->queue_lock));

        /*调用回调函数,执行任务*/
        (*(worker->Request))(worker->arg);
        free(worker);
        worker = NULL;
    }
    pthread_exit(NULL);
    return NULL;
}

void *Request(void* arg)
{
    printf("%d,tid:[%lu]\n",*(int*)arg,pthread_self());
    sleep(1);
    return NULL;
}


//int main()
//{
//    PoolInit(3);/*线程池中最多三个活动进程*/
//
//    /*连续向池中投入10个任务*/
//    int *workingnum = (int *) malloc (sizeof (int) * 10);  
//    int i;  
//    for (i = 0; i < 10; i++)  
//    {  
//        workingnum[i] = i;
//        PoolAddWorker(Request, &workingnum[i]);
//    }  
//    /*等待所有任务完成*/  
//    sleep (5);  
//    /*销毁线程池*/  
//    PoolDestroy();  
//
//    free(workingnum);
//    return 0;
//}
//
