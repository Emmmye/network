#pragma once

#include <iostream>
#include <sys/time.h>


class TimeUtil{
public:
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
};


enum LogLevel{
    INFO,//信息
    WARNING,//警告
    ERROR, //错误
    CRITIAL,//致命缺陷
};

// LOG(INFO) << "HELLO"
// [I时间戳 util.hpp:31] HELLO

inline std::ostream& Log(LogLevel level, const char* file, int line){
    std::string prefix = "I";
    if(level == WARNING){
        prefix = "W";
    }else if(level == ERROR)
    {
        prefix = "E";
    }else if(level == CRITIAL){
        prefix = "C";
    }

    std::cout << "[" << prefix << TimeUtil::TimeStamp() << " " << file << ":" << line << "]";

    return std::cout;
}

// __FILE__ __LINE__
#define LOG(level) Log(level,__FILE__, __LINE__)
