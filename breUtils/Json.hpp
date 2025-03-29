#pragma once
/**
    @file Json.hpp
    @brief A lightweight JSON parsing library header file.

    This library provides a simple JSON parsing solution for small projects 
    without the need to introduce thirdparty libraries. It is designed for 
    scenarios where JSON parsing requirements are minimal and portability 
    is a concern. For more complex JSON parsing needs, consider using 
    thirdparty libraries.

    @note By default, strict mode is disabled. In strict mode, comments 
    and trailing commas in JSON strings are not allowed.

    Usage

    Parsing JSON Strings
    Parsing a JSON string requires a trycatch block, as parsing errors 
    will throw exceptions.

    try {
        bre::json::Value json = bre::Json::parse(jsonStr);
    } catch (const bre::JsonParseException& e) {
        std::cout << e.what() << std::endl;
    }

    Accessing JSON Values
    Once parsed, the JSON object can be accessed and manipulated as follows:

    Accessing Values:
    For objects: `json["key"]`
    For arrays: `json[0]`

    Setting Values:
    For objects: `json["key"] = Value(123);`
    For arrays: `json.Append(Value(123));`

    Type Conversion:
    Use `json.AsXXX()` to retrieve values of specific types.

    Iterators:
    For arrays: `for (auto& item : json.GetArray())`
    For objects: `for (auto& item : json.GetObject())`

    Outputting JSON
    To output the JSON object as a string:
    Without indentation: `json.toString()`
    With indentation: `json.ToStyledString()`

    Example:
    std::cout << json << std::endl; // Outputs JSON string with indentation

    @warning The JSON file being parsed should not be excessively large, 
        as the entire string is loaded into memory during parsing.
*/

/**
    @file Json.hpp
    @brief 一个轻量级的 JSON 解析库头文件。

    该库为小型项目提供了简单的 JSON 解析解决方案，无需引入第三方库。
    它适用于 JSON 解析需求较少且需要关注可移植性的场景。
    对于更复杂的 JSON 解析需求，请考虑使用第三方库。

    @note 默认情况下，严格模式是关闭的。在严格模式下，JSON 字符串中不允许有注释和尾随逗号。

    使用方法

    解析 JSON 字符串
    解析 JSON 字符串需要使用 trycatch 块，因为解析错误会抛出异常。
    
    try {
        bre::json::Value json = bre::Json::parse(jsonStr);
    } catch (const bre::JsonParseException& e) {
        std::cout << e.what() << std::endl;
    }

    访问 JSON 值
    解析后，可以如下访问和操作 JSON 对象：
    
    访问值：
        对于对象: `json["key"]`
        对于数组: `json[0]`
    
    设置值：
        对于对象: `json["key"] = Value(123);`
        对于数组: `json.Append(Value(123));`
    
    类型转换：
        使用 `json.AsXXX()` 获取特定类型的值。

    迭代器：
        对于数组: `for (auto& item : json.GetArray())`
        对于对象: `for (auto& item : json.GetObject())`

    输出 JSON
    将 JSON 对象输出为字符串：
        无缩进: `json.toString()`
        带缩进: `json.ToStyledString()`
    
    示例：
    std::cout << json << std::endl; // 输出带缩进的 JSON 字符串

    @warning 被解析的 JSON 文件不应过大，因为解析时整个字符串会被加载到内存中。
*/

#include "./Json/json_tool.hpp"

