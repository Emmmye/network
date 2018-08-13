#include <signal.h>
#include "chat_client.h"
#include "window.h"
#include "../common/util.hpp"
#include "../common/api.hpp"

client::ChatClient* g_client = NULL;
client::Window* g_window = NULL;
pthread_mutex_t g_lock;

void Quit(int arg)
{
    (void)arg;
    g_client->SendMsg("quit");
    delete g_window;
    delete g_client;
    pthread_mutex_destroy(&g_lock);
    exit(0);
}

void* Send(void* arg)
{
    // 读取输入数据,发送给服务器
    (void)arg;
    while(1){
        std::string msg;
        pthread_mutex_lock(&g_lock);
        g_window->DrawInput();
        pthread_mutex_unlock(&g_lock);
        g_window->GetStrFromWin(g_window->input_win,&msg);
        g_client->SendMsg(msg);
        if(msg == "quit")
        {
            return NULL;
        }
    }
    return NULL;
}
void* Recv(void* arg)
{
    (void)arg;
    // 从服务器读取数据,显示到屏幕上
    while(1){
        pthread_mutex_lock(&g_lock);
        g_window->DrawOutPut();
        pthread_mutex_unlock(&g_lock);
        g_window->DrawFriendList();
        server::Data data;
        g_client->RecvMsg(&data);
        if(data.cmd == "quit"){
            //保证插入和删除的方式能对应上
            if(data.name == g_client->GetName())
            {
                return NULL;
            }
            g_window->DelFriend(data.name + "|" + data.school);
        }else{
            g_window->AddFriend(data.name + "|" + data.school);
            g_window->AddMsg(data.name + ":" + data.msg);
        }
    }
    return NULL;
}

// google 常见的C++开源库
// protobuf: 用来序列化
// glog: 打日志
// gflag: 配置项管理
// grpc: RPC框架(协议)  
// gtest: 单元测试框架

void Run(const std::string& ip, short port)
{
    // 捕捉SIGINT信号
    // 如果不捕捉SIGINT信号,在收到该信号后,进程退出,g_window不会调用析构函数，终端页面将会混乱 
    signal(SIGINT,Quit);

    pthread_mutex_init(&g_lock,NULL);
    // 1. 初始化客户端核心模块
    g_client = new client::ChatClient();
    int ret = g_client->Init(ip,port);
    if(ret < 0){
        // 这里模仿了 Google 的 glog 库
        LOG(ERROR) << "client Init failed!\n";
        return;
    }

    // 2. 提示用户输入用户名和学校
    std::string name, school;
    std::cout << "请输入用户名:" << std::endl;
    std::cin >> name;
    std::cout << "请输入学校:" << std::endl;
    std::cin >> school;
    g_client->SetUserInfo(name,school);

    // 3. 初始化用户界面模块
    g_window = new client::Window();
    g_window->DrawHeader();

    // 4. 创建两个线程:
    pthread_t stid,rtid;
    //   a) 发送线程: 读取输入数据，发送给服务器
    pthread_create(&stid,NULL,Send,NULL);
    //   b) 接收线程: 从服务器中读取数据，显示到界面上
    pthread_create(&rtid,NULL,Recv,NULL);
    pthread_join(stid,NULL);
    pthread_join(rtid,NULL);
    // 5. 对进程退出时进行处理
    delete g_window;
    delete g_client;
    pthread_mutex_destroy(&g_lock);
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        LOG(ERROR) << "Usage ./chat_client_main [ip] [port]\n";
        return 1;
    }

    Run(argv[1], atoi(argv[2]));
    return 0;
}
