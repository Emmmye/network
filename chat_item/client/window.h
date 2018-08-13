#pragma once
#include <ncurses.h>
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <queue>
#include <unordered_set>

//vim 就是基于 ncurses 实现的

namespace client{
class Window{
public:
    Window();
    ~Window();
    //绘制标题框
    void DrawHeader();
    //绘制输入框
    void DrawInput();
    //绘制输出框
    void DrawOutPut();
    //绘制好友列表
    void DrawFriendList();
    //往窗口中写字符串
    void PutStrToWin(WINDOW* win, int y, int x, std::string& str);
    //从窗口中读字符串
    void GetStrFromWin(WINDOW* win, std::string* str);

    //往消息窗口中添加消息
    void AddMsg(const std::string& msg);

    //删除和添加好友
    void AddFriend(const std::string& frinend_info);
    void DelFriend(const std::string& frinend_info);
    WINDOW* header_win;
    WINDOW* input_win;
    WINDOW* output_win;
    WINDOW* frinend_list_win;

private:
    // 保存当前要显示的消息
    // 消息填充满窗口后，要删除最上面的一条
    // deque 勉强支持随机访问
    std::deque<std::string> msgs_;
    // 当前的在线好友列表
    std::unordered_set<std::string> frinend_list;
};
}// end client
