#pragma once
#include <string>
#include <json/json.h>
namespace server{

//此处的Data相当于服务器给客户端提供的API
struct Data{
    std::string name;
    std::string school;
    std::string msg;
    //对于当前的情况下, cmd 的取值要么为空字符串,
    //要么为 "quit" 表示退出
    std::string cmd;//控制字段

    //序列化
    void Serialize(std::string* output){
        //可以把 Json::Value 近似理解成一个 unordered_map
        //接下来把数据插入到value中
        Json::Value value;
        value["name"] = name;
        value["school"] = school;
        value["msg"] = msg;
        value["cmd"] = cmd;
        Json::FastWriter writer;
        *output = writer.write(value);
        return;
    }

    //反序列化
    void UnSerialize(const std::string& input){
        Json::Value value;
        Json::Reader reader;
        reader.parse(input,value);
        if(value["name"].isString()){
            name = value["name"].asString();
        }
        if(value["school"].isString()){
            school = value["school"].asString();
        }
        if(value["msg"].isString()){
            msg = value["msg"].asString();
        }
        if(value["cmd"].isString()){
            cmd = value["cmd"].asString();
        }
        return;
    }

};
} // end server
