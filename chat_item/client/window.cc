#include <locale.h>
#include "../common/util.hpp"
#include "window.h"


namespace client{
    
Window::Window()
{
    //设置编码格式支持中文
    setlocale(LC_ALL,"");
    //初始化整个 ncurses
    initscr();
    //隐藏光标
    curs_set(0);
}
    
Window::~Window()
{
    // 销毁整个 ncurses
    endwin();
}

void Window::PutStrToWin(WINDOW* win, int y, int x, std::string& str)
{
    mvwaddstr(win,y,x,str.c_str());
}
   
void Window::GetStrFromWin(WINDOW* win, std::string* str)
{
    char buf[1024 * 10] = {0};
    wgetnstr(win,buf,sizeof(buf)-1);
    *str = buf;
}


void Window::DrawHeader()
{
    //LINES ncurses 提供的宏,表示当前窗口的最大行数
    int h = LINES / 5;
    // COLS 也是一个宏,表示当前窗口的最大列数
    int w = COLS;
    // 窗口左上角的坐标
    int y = 0;
    int x = 0;
    header_win = newwin(h,w,y,x);
    //绘制窗口边框
    box(header_win,'|','-');
    std::string str("这是一个不太挫的聊天室");
    PutStrToWin(header_win, h / 2, w / 2 - 11, str);
    //窗口刷新
    wrefresh(header_win);
}

void Window::DrawInput()
{
    int h = LINES / 5;
    int w = COLS;
    int y = LINES * 4 / 5;
    int x = 0;
    input_win = newwin(h,w,y,x);
    //绘制窗口边框
    box(input_win,'|','-');
    std::string str("请输入消息:");
    // 这是相对坐标系
    // 也就是窗口左上角为原点的坐标系
    PutStrToWin(input_win, 1, 2, str);
    //窗口刷新
    wrefresh(input_win);
}

void Window::DrawOutPut()
{
    int h = LINES * 3 / 5;
    int w = COLS * 3 / 4;
    int y = LINES  / 5;
    int x = 0;
    output_win = newwin(h,w,y,x);
    //绘制窗口边框
    box(output_win,'|','-');
    size_t j = 0;
    for(size_t i = 0; i < msgs_.size(); ++i)
    {
        if((int)msgs_[i].size() >= COLS*3/4){
            std::string new_str;
            new_str.assign(msgs_[i],0,(COLS*3/4) - 3);
            PutStrToWin(output_win,j+1,2,new_str);
            msgs_[i].erase(0,(COLS*3/4 - 3));
            ++j;
            PutStrToWin(output_win,j+1,2,msgs_[i]);
        }
        else{
            PutStrToWin(output_win,j+1,2,msgs_[i]);
        }
        j++;
    }
    //窗口刷新
    wrefresh(output_win);
}
void Window::DrawFriendList()
{
    int h = LINES * 3 / 5;
    int w = COLS  / 4;
    int y = LINES  / 5;
    int x = COLS * 3 / 4;
    frinend_list_win = newwin(h,w,y,x);
    //绘制窗口边框
    box(frinend_list_win,'|','-');
    std::string str("好友列表:");
    // 这是相对坐标系
    // 也就是窗口左上角为原点的坐标系
    PutStrToWin(frinend_list_win, 1, 2, str);
    
    //遍历好友列表,把好友信息显示上来
    size_t i = 0;
    for(auto item : frinend_list)
    {
        i++;
        PutStrToWin(frinend_list_win,i+1,1,item);
    }
    wrefresh(frinend_list_win);
}

void Window::AddMsg(const std::string& msg)
{
    // 由于窗口显示消息条数有限,所以要约定好窗口上限
    // 当 msgs 包含的消息数目超过一定的阈值，就把旧消息删除掉
    // 如果要想实现历史消息查询等，就把消息写入到文件中或者插入到数据库中
    msgs_.push_back(msg);
    int max_line = LINES * 3 / 5 - 2;
    if(max_line < 3){
        LOG(ERROR) << "max_line too small\n";
        exit(0);
    }
    if((int)msgs_.size() > max_line){
        msgs_.pop_front();
    }
}
void Window::AddFriend(const std::string& frinend_info)
{
    frinend_list.insert(frinend_info);
}
void Window::DelFriend(const std::string& frinend_info)
{
    frinend_list.erase(frinend_info);
}


}

#if 0
///////////////////////////////////////////
//测试 窗口
//////////////////////////////////////////


int main()
{
    client::Window win;
    win.DrawHeader();
    win.DrawInput();
    win.AddMsg("1111:111");
    win.AddMsg("2222:222");
    win.AddMsg("3333:hehehhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhaaaaaaaaaaaaaaaaaabbbbbbbbbb");
    win.AddMsg("4444:heha");
    win.DrawOutPut();
    win.AddFriend("55555");
    win.AddFriend("55555");
    win.AddFriend("55555");
    win.DrawFriendList();
    sleep(90);
    return 0;
}
#endif
