#pragma once
#include <vector>
#include <semaphore.h>
//函数和定义在一起
//阻塞队列

namespace server{
//类模板 这是一个模板，模板实例化的时候会对应生成有具体类型的类的代码
//源编程 由代码生成代码
template <typename T>
class BlockQueue{
public:
    BlockQueue(size_t s = 1024)
        :_data(s),
        _head(0),
        _tail(0),
        _size(0)
    {
        sem_init(&_sem_data, 0, 0);//第二个参数填0表示用于线程间，填其他的可以表示用于进程间
        sem_init(&_sem_blank, 0, s);//表示队列中有多少空位可以用
    }

    ~BlockQueue()
    {
        //销毁信号量
        sem_destroy(&_sem_data);
        sem_destroy(&_sem_blank);
    }
    void Push(const T& value)
    {
        sem_wait(&_sem_blank);
        _data[_tail++] = value;
        if(_tail >= _data.size())
        {
            _tail = 0;
        }
        ++_size;
        sem_post(&_sem_data);
    }
    void Pop(T* value)
    {
        sem_wait(&_sem_data);
        *value = _data[_head++];
        if(_head >= _data.size())
        {
            _head =  0;
        }
        --_size;
        sem_post(&_sem_blank);
    }
private:
    std::vector<T> _data;
    sem_t _sem_data;
    sem_t _sem_blank;
    //队头
    size_t _head;
    //队尾
    size_t _tail;
    //[head,tail)
    //队列有效元素数量
    size_t _size;
    //由于我们此处只实现一个单生产者单消费者的BlockQueue
    //故可以不加互斥锁
    //sem_t _sem_lock;//二元信号量作为互斥锁
};
}
