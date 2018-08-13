#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <unordered_map>
#include "block_queue.hpp"
#include "../common/api.hpp"
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

namespace server{
    
struct Context{
    std::string str;
    sockaddr_in addr;
};

class ChatServer{
public:
    //启动服务器
    int Start(const std::string& ip, short port);
    //接收消息
    int RecvMsg();
    //广播消息
    int BroadCast();

private:
    static void* Consume(void*);
    static void* Product(void*);

    void AddUser(sockaddr_in addr,const std::string& name);
    void DelUser(sockaddr_in addr,const std::string& name);

    void SendMsg(const std::string& data, sockaddr_in addr);

    //key 内容用户身份标识，ip+昵称
    //value ip+端口号(struct sockaddr_in)
    std::unordered_map<std::string,sockaddr_in> _online_friend_list;
    // TODO 实现一个阻塞队列，作为交易场所
    // 暂定队列中元素类型为 std::string
    BlockQueue<Context> _queue;
    int _sock; //服务器进行绑定的文件描述符
};


}// end server
