#pragma once
/*
仅头文件Json解析写入库，使用时包含此头文件即可
注意：Json解析的文件不应该过大，解析会把整个字符串读入内存

使用方式：
    bre::Value json = bre::Json::parse(jsonStr);
    json就是一个Json对象，可以通过json["key"]获取值
    cout << json << endl; // 输出json字符串（带缩进）
    json toString()和ToStyledString()可以获取json字符串，前者不带缩进，后者带缩进
*/

#include "./Json/json_tool.hpp"

