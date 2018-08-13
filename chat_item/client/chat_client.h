#pragma once
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

#include "../common/api.hpp" 
typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;


namespace client{
class ChatClient{
public:
    //客户端初始化，主要是进行socket的初始化
    int Init(const std::string& ip, short port);
    //设置用户信息,姓名和学校
    //可以从标准输入读取
    int SetUserInfo(const std::string& name, const std::string& school);
    //发送消息
    void SendMsg(const std::string& msg);
    //接收消息
    //本质上接收到的内容也是 JSON 格式的内容
    void RecvMsg(server::Data* data);
    //获取用户姓名
    std::string GetName()
    {
        return  _name;
    }
private:
    // 这个文件描述符用来保存和服务器交互的 socket
    int _sock;
    sockaddr_in _server_addr;

    std::string _name;
    std::string _school;
};
} //end client
