#pragma once

#include "json_value.hpp"
#include "json_exception.hpp"
#include <string>

namespace bre {
namespace json {
    class Generator {
    public:
        Generator() = default;

        // 生成 JSON 字符串 (pretty = true 表示生成带缩进的字符串)
        static std::string generate(const Value& val, bool pretty = true, int indentWidth = 2) {
            Generator generator(pretty ? indentWidth : 0);
            return generator.generateValue(val, 0);
        }

    private:
        explicit Generator(int indentWidth) : indentWidth_(indentWidth) {}

        std::string indent(int level) const {
            return std::string(level * indentWidth_, ' ');
        }

        std::string generateValue(const Value& val, int level) {
            switch (val.type()) {
            case Type::Null:   return "null";
            case Type::Bool:   return val.AsBool() ? "true" : "false";
            case Type::Int:    return std::to_string(val.AsInt());
            case Type::Double: return std::to_string(val.AsDouble());
            case Type::String: return "\"" + escapeString(val.AsString()) + "\"";
            case Type::Array:  return generateArray(val.AsArray(), level + 1);
            case Type::Object: return generateObject(val.AsObject(), level + 1);
            default: throw JsonParseException("Invalid Value type");
            }
        }

        std::string generateArray(const Value::Array& array, int level) {
            if (array.empty()) return "[]";

            std::string result = "[";
            if (indentWidth_ > 0) result += "\n";

            for (size_t i = 0; i < array.size(); ++i) {
                if (indentWidth_ > 0) result += indent(level);
                result += generateValue(array[i], level);
                if (i < array.size() - 1) {
                    result += ",";
                }
                if (indentWidth_ > 0) result += "\n";
            }

            if (indentWidth_ > 0) result += indent(level - 1);
            result += "]";
            return result;
        }

        // 锟斤拷锟斤拷 JSON `锟斤拷锟斤拷` 锟街凤拷锟斤拷
        std::string generateObject(const Value::Object& object, int level) {
            if (object.empty()) return "{}";

            std::string result = "{";
            if (indentWidth_ > 0) result += "\n";

            size_t count = 0;
            for (const auto& [key, value] : object) {
                if (indentWidth_ > 0) result += indent(level);
                result += "\"" + escapeString(key) + "\": " + generateValue(value, level);
                if (++count < object.size()) {
                    result += ",";
                }
                if (indentWidth_ > 0) result += "\n";
            }

            if (indentWidth_ > 0) result += indent(level - 1);
            result += "}";
            return result;
        }

        // 转锟斤拷 `锟街凤拷锟斤拷` 锟叫碉拷锟斤拷锟斤拷锟街凤拷
        std::string escapeString(const std::string& s) {
            std::string escaped;
            for (char c : s) {
                switch (c) {
                case '"': escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\b': escaped += "\\b"; break;
                case '\f': escaped += "\\f"; break;
                case '\n': escaped += "\\n"; break;
                case '\r': escaped += "\\r"; break;
                case '\t': escaped += "\\t"; break;
                default: escaped += c; break;
                }
            }
            return escaped;
        }

        int indentWidth_; // 锟斤拷锟斤拷锟斤拷锟斤拷
    };
} // namespace json
} // namespace bre

