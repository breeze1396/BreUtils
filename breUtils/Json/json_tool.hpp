#pragma once
#ifndef JSON_TOOL_HPP
#define JSON_TOOL_HPP

#include "json_value.hpp"
#include "json_generator.hpp"
#include "json_exception.hpp"
#include "json_parse.hpp"

// #include <string>
// #include <cctype>
// #include <stdexcept>
// #include <sstream>

namespace bre {
namespace json {
    inline std::ostream& operator<<(std::ostream& os, const Value& val) {
        os << Generator::generate(val);
        return os;
    }

    // 生成 JSON 字符串，不带缩进
    inline std::string Value::ToString() const {
        return json::Generator::generate(*this, false);
    }

    // 生成 JSON 字符串，带缩进
    inline std::string Value::ToStyledString() const {
        return json::Generator::generate(*this);
    }

// 递归比较两个 Value 对象是否相等
} // namespace json
} // namespace bre
    
inline bool bre::json::Value::operator==(const Value& other) const{
    if (type_ != other.type_) {
        return false;
    }

    switch (type_) {
    case Type::Null:
        return true;
    case Type::Int:
        return this->AsInt() == other.AsInt();
    case Type::Double:
        return this->AsDouble() == other.AsDouble();
    case Type::Bool:
        return this->AsBool() == other.AsBool();
    case Type::String:
        return this->AsString() == other.AsString();
    case Type::Array:
        {
            auto thisArray = this->AsArray();
            auto otherArray = other.AsArray();
            if (thisArray.size() != otherArray.size()) {
                return false;
            }
            for (size_t i = 0; i < thisArray.size(); ++i) {
                if (!thisArray[i].operator==(otherArray[i])) {
                    return false;
                }
            }
            return true;
        }
    case Type::Object:
        {
            auto thisObject = this->AsObject();
            auto otherObject = other.AsObject();
            if (thisObject.size() != otherObject.size()) {
                return false;
            }
            return thisObject == otherObject;
        }
    default:
        return false;
    }
    return false;
}


#endif // JSON_TOOL_HPP
