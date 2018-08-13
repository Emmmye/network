#include <iostream>
#include <sstream>
#include "chat_server.h"
#include "../common/util.hpp"
#include "../common/api.hpp"
namespace server{

int ChatServer::Start(const std::string& ip, short port)
{
    //启动服务器，并且进入事件循环
    //使用UDP作为我们的通信协议
    _sock = socket(AF_INET,SOCK_DGRAM,0);
    if(_sock < 0){
        perror("socket");
        return -1;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = port;

    int ret = bind(_sock,(sockaddr*)&addr,sizeof(addr));
    if(ret < 0)
    {
        perror("bind");
        return -1;
    }
    LOG(INFO) << "Server Start OK!\n";
    pthread_t productor,consumer;
    pthread_create(&productor,NULL,Product,this);
    pthread_create(&consumer,NULL,Consume,this);
    pthread_join(productor,NULL);
    pthread_join(consumer,NULL);
    return 0;
}

void* ChatServer::Product(void* arg)
{
    //生产者线程，读取socket，将消息写入队列BlockQueue中
    ChatServer* server = reinterpret_cast<ChatServer*>(arg);
    while(true){
        server->RecvMsg();
    }
    return NULL;
}

void* ChatServer::Consume(void* arg)
{
    ChatServer* server = reinterpret_cast<ChatServer*>(arg);
    while(true){
        server->BroadCast();
    }
    return NULL;
}
void ChatServer::AddUser(sockaddr_in addr,const std::string& name)
{
    //这里的key相当于对ip地址和用户名进行拼接
    //之所以使用name和ip地址共同进行拼接，本质上是因为仅仅使用ip可能会
    //出现重复ip的情况(如果N个客户端处在同一个局域网中，那么服务器端看
    //到的ip地址就是相同.因为NAT技术,源ip地址会被替换为局域网出口路由器的ip)
    std::stringstream ss;
    ss << name << ":" << addr.sin_addr.s_addr;
    // [] map unordered_map
    // 1.如果不存在，就插入 
    // 2.如果存在，就修改
    // ValueType& operator[](const KeyType& key)
    // const ValueType& operator[](const KeyType& key) const
    //
    // insert 返回值类型: 迭代器，插入成功后的位置;bool 成功与失败
    _online_friend_list[ss.str()] = addr;
}

void ChatServer::DelUser(sockaddr_in addr,const std::string& name)
{
    std::stringstream ss;
    ss << name << ":" << addr.sin_addr.s_addr;
    _online_friend_list.erase(ss.str());
}

int ChatServer::RecvMsg(){
    // 1.从socket中读取数据
    char buf[1024 * 5] = {0};
    sockaddr_in peer;
    socklen_t len = sizeof(peer);

    ssize_t read_size = recvfrom(_sock,buf,sizeof(buf)-1,0,(sockaddr*)&peer,&len);
    
    if(read_size < 0){
        perror("recvfrom");
        return -1;
    }
    buf[read_size] = '\0';
    LOG(INFO) << "[Request] " << buf << "\n";
    Context context;
    context.str = buf;
    context.addr = peer;
    // 2.把数据插入到BlockQueue中
    _queue.Push(context);
    return 0;
}


void ChatServer::SendMsg(const std::string& data, sockaddr_in addr)
{
    // 把这个数据通过 sendto 发送给客户端
    sendto(_sock,data.c_str(),data.size(),0,(sockaddr*)&addr,sizeof(addr));
    LOG(INFO) << "[Response] " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << " " << data << "\n";
}

int ChatServer::BroadCast()
{
    //1.从队列中读取一个元素
    Context context;
    _queue.Pop(&context);

    //2.对取出的字符串数据进行反序列化
    Data data;
    data.UnSerialize(context.str);

    //3.根据这个消息更新在线好友列表
    // a)如果这个成员当前是一个下线的消息(command是一个特殊的值)，
    //   把这样的一个成员从好友列表中删除;
    if(data.cmd == "quit")
    {
        DelUser(context.addr,data.name);
    }
    else
    {
    // b)如果这个成员当前不在好友列表中，就添加进去
    // 准备使用[]方法 来操作在线好友列表
        AddUser(context.addr,data.name);
    }
    
    //4.遍历在线好友列表，把这个数据依次发送给每一个客户端.
    //  (由于发送消息的用户也存在与好友列表中，因此在遍历列表时
    //   也会给自己发送消息，从而达到发送者能够看到自己发送的消息
    //   的效果，但是这种实现方式不太好，完全可以控制客户端，不经过
    //   网络传输就实现这个功能，实现优化)
    for(auto item : _online_friend_list)
    {
        SendMsg(context.str,item.second);
    }
    return 0;
}

}// end server
