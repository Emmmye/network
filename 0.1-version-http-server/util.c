#include "util.h"
int GetLine(int sock, char buf[], int size)
{
    int i = 0;
    char ch = 'a';
    ssize_t s = 0;
    while(ch != '\n' && i < size)
    {   
        s = recv(sock,&ch,1,0);
        if(s > 0){
            if(ch == '\r'){
                //窥探 sock 里的下一字符
                //并没有真的读出来
                if(recv(sock,&ch,1,MSG_PEEK) > 0)
                {
                    if(ch == '\n')
                        recv(sock,&ch,1,0);
                    ch = '\n';
                }   
            }
            buf[i++] = ch;
        }
        else
            break;
    }
    buf[i] = '\0';
    return i;
}

//将请求报头剩余部分一次性读完
void ClearHeader(int sock)
{
    char buf[MAX_LINE];
    do{
        if((GetLine(sock,buf,MAX_LINE)) <= 0)
            break;
    }while(strcmp(buf,"\n") != 0);
}

//读取配置文件
int ReadEtc()
{
    char buf[10] = {0};
    int etc_fd = open("./server_etc",O_RDONLY);
    int read_size = read(etc_fd,buf,sizeof(buf)-1);
    if(read_size <= 0)
    {
        exit(1);
    }
    buf[read_size] = '\0';
    return atoi(buf);
    close(etc_fd);
}


//打印日志
//获取到当前的秒级时间戳
static int64_t TimeStamp(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec;
}
//获取到当前的微秒级时间戳
static int64_t TimeStampUS(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return 1000 * 1000 * tv.tv_sec + tv.tv_usec;
}

// __FILE__ __LINE__
#define LOG(level) Log(level,__FILE__, __LINE__)


// LOG(INFO) << "HELLO"
// [I时间戳 util.hpp:31] HELLO
void Log(enum LogLevel level, char* file, int line)
{
    FILE* f_log = fopen("./log","a");
    if(f_log == NULL)
    {
        perror("fopen");
        return;
    }
    char prefix = 'I';
    if(level == ERROR){
        prefix = 'E';
    }else if(level == WARNING)
    {
        prefix = 'W';
    }else if(level == CRITIAL)
    {
        prefix = 'C';
    }

    char LogBuf[MAX_LINE] = {0};
    sprintf(LogBuf, "[%c | %lu | %d | %s]\n",prefix,TimeStamp(),line,file);
    fwrite(LogBuf,strlen(LogBuf),1,f_log);
    fflush(f_log);
    fclose(f_log);
}


//信号处理函数
void HandleForSigpipe()
{
    struct sigaction sa;
    memset(&sa, '\0',sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE,&sa,NULL))
        return;
}

//将socket设置为非阻塞模式
int SetSocketNoBlock(int sock)
{
    int flag = fcntl(sock,F_GETFL,0);
    if(flag == -1)
        return -1;

    flag |= O_NONBLOCK;
    if(fcntl(sock,F_SETFL,flag) == -1)
        return -1;
    return 0;
}



