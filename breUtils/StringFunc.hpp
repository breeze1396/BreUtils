#pragma once
#include "String/base.hpp"

/*
1. 分割与连接
split(const std::string&, char delimiter)
根据单个字符分隔符分割字符串，返回vector<string>。
split(const std::string&, const std::string& delimiter)
根据字符串分隔符分割字符串，返回vector<string>。
join(const std::vector<std::string>&, const std::string& separator)
用分隔符连接字符串列表，返回拼接后的字符串。

2. 查找与替换
find_all(const std::string&, const std::string&)
返回所有匹配子串的起始位置的vector<size_t>。
replace_all(std::string&, const std::string&, const std::string&)
替换字符串中所有匹配子串。
replace_first(std::string&, const std::string&, const std::string&)
替换字符串中第一个匹配子串。

3. 大小写转换
to_upper(std::string&)
将字符串转为大写（修改原字符串）。
to_lower(std::string&)
将字符串转为小写（修改原字符串）。
to_upper_copy(const std::string&)
返回大写的新字符串（不修改原字符串）。
to_lower_copy(const std::string&)
返回小写的新字符串（不修改原字符串）。

4. 检查函数
starts_with(const std::string&, const std::string&)
检查字符串是否以指定前缀开头。
ends_with(const std::string&, const std::string&)
检查字符串是否以指定后缀结尾。
contains(const std::string&, const std::string&)
检查字符串是否包含指定子串。

5. Trim函数
trim(std::string&)
去除字符串前后空格（修改原字符串）。
ltrim(std::string&)
去除字符串左端空格（修改原字符串）。
rtrim(std::string&)
去除字符串右端空格（修改原字符串）。
trim_copy(const std::string&)
返回去除前后空格的新字符串（不修改原字符串）。
6. 转换函数

to_int(const std::string&)
将字符串转为整数（抛出异常或返回错误码）。
to_double(const std::string&)
将字符串转为双精度浮点数。
to_string(double)
将双精度浮点数转为字符串（可指定精度）。

7. 其他实用函数
pad_left(const std::string&, size_t width, char pad_char)
左填充字符串至指定长度。
pad_right(const std::string&, size_t width, char pad_char)
右填充字符串至指定长度。
reverse(const std::string&)
返回反转后的字符串。
*/

class StringFunc
{
public:
    
};

