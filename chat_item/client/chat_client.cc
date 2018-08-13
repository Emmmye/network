#include "chat_client.h"
namespace client{

// ip 和 port 通过命令行参数来传
// 优化：从配置文件中读取 ip 和 port
// 服务器一启动，就打开配置文件进行读取
// 定义好文件格式，比如说json格式
int ChatClient::Init(const std::string& server_ip, short server_port)
{
    _sock = socket(AF_INET,SOCK_DGRAM,0);
    if(_sock < 0){
        perror("socket");
        return -1;
    }

    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
    _server_addr.sin_port = htons(server_port);
    return 0;
}

int ChatClient::SetUserInfo(const std::string& name, const std::string& school)
{
    _name = name;
    _school = school;
    return 0;
}

void ChatClient::SendMsg(const std::string& msg)
{
    //客户端和服务器交互的接口要严格按照服务器提供的 api 来操作
    //相当于自定义应用层协议
    //数据准备
    server::Data data;
    data.name = _name;
    data.school = _school;
    data.msg = msg;
    if(msg == "quit" || msg == "exit" ){
        data.cmd = "quit";
    }else{
        data.cmd = "";
    }

    //序列化
    std::string str;
    data.Serialize(&str);

    //发送数据
    sendto(_sock,str.c_str(),str.size(),0,(sockaddr*)&_server_addr,sizeof(_server_addr));
    return;
}

void ChatClient::RecvMsg(server::Data* data)
{
    //定义一个足够大的缓冲区
    char buf[1024 * 10] = {0};
    // 由于对端的ip端口号就是服务器的ip端口号，所以此处
    // 没必要再获取一遍对端的ip，这部分内容已经都知道了
    ssize_t read_size = recvfrom(_sock,buf,sizeof(buf)-1,0,NULL,NULL);

    if(read_size < 0)
    {
        perror("recvfrom");
        return;
    }
    buf[read_size] = '\0';

    //反序列化
    data->UnSerialize(buf);
    return;
}

}

//////////////////////////////////////////////////////
// 测试版本
// ///////////////////////////////////////////////////

#if 0
#include <iostream>
#include <pthread.h>

void *Send(void* arg)
{
    //循环发送数据
    client::ChatClient* client = reinterpret_cast<client::ChatClient*>(arg);
    while(true)
    {
        std::string str;
        std::cin >> str;
        client->SendMsg(str);
    }
    return NULL;
}

void *Recv(void* arg)
{
    //循环接收数据
    client::ChatClient* client = reinterpret_cast<client::ChatClient*>(arg);
    while(true){
        server::Data data;
        client->RecvMsg(&data);
        std::cout << "[" << data.name << "]" << "[" << data.school << "]" << ":" << data.msg << std::endl;
    }
    return NULL;
}


int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Usage ./client [ip] [port]\n");
        return 1;
    }
    client::ChatClient client;
    client.Init(argv[1],atoi(argv[2]));

    std::cout << "输入用户名: ";
    std::string name;
    std::cin >> name;
    std::cout << "输入学校: ";
    std::string school;
    std::cin >> school;
    client.SetUserInfo(name,school);

    // 创建两个线程，分别用于发送数据和接收数据
    pthread_t stid,rtid;
    pthread_create(&stid,NULL,Send,&client);
    pthread_create(&rtid,NULL,Recv,&client);
    pthread_join(stid,NULL);
    pthread_join(rtid,NULL);
    return 0;
}

#endif
