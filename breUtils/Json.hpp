#pragma once
/*
仅头文件 Json库，使用时包含此头文件即可
主要目的是针对一个不大的项目，提供一个简单的Json解析库，不用引入第三方库移植的问题
如果一个功能专门做Json解析，次库可能不够用，可以考虑使用第三方库
注意：Json解析的文件不应该过大，解析会把整个字符串读入内存

使用方式：
    解析Json字符串需要try-catch, 因为解析失败会抛出异常
    try {
        bre::Value json = bre::Json::parse(jsonStr);
    } catch (const bre::JsonParseException& e) {
        cout << e.what() << endl;
    }

    json就是一个Json对象，可以通过
        json.AsXXX()获取值    
        针对对象：可以json["key"]获取值
        针对数组：可以json[0]获取值
        针对对象：可以json["key"] = Value(123)设置值
        针对数组：可以json.Append(Value(123))添加值
    cout << json << endl; // 输出json字符串（带缩进）
    json toString()和ToStyledString()可以获取json字符串，前者不带缩进，后者带缩进
*/

#include "./Json/json_tool.hpp"

