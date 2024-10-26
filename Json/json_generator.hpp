#ifndef JSON_GENERATOR_HPP
#define JSON_GENERATOR_HPP

#include "json_value.hpp"
#include "json_exception.hpp"
#include <string>

namespace bre {

    class Generator {
    public:
        Generator() = default;

        // 生成json字符串, pretty为false时，为压缩状态
        static std::string generate(const Value& val, bool pretty = true, int indentWidth = 2) {
            Generator generator(pretty ? indentWidth : 0);
            return generator.generateValue(val, 0);
        }

    private:
        explicit Generator(int indentWidth) : indentWidth_(indentWidth) {}

        // 生成缩进字符串
        std::string indent(int level) const {
            return std::string(level * indentWidth_, ' ');
        }

        // 递归生成 JSON，若无缩进宽度即为压缩格式
        std::string generateValue(const Value& val, int level) {
            switch (val.type()) {
            case Type::Null:   return "null";
            case Type::Bool:   return val.asBool() ? "true" : "false";
            case Type::Int:    return std::to_string(val.asInt());
            case Type::Double: return std::to_string(val.asDouble());
            case Type::String: return "\"" + escapeString(val.asString()) + "\"";
            case Type::Array:  return generateArray(val.asArray(), level + 1);
            case Type::Object: return generateObject(val.asObject(), level + 1);
            default: throw JsonParseException("Invalid Value type");
            }
        }

        // 生成 JSON `数组` 字符串
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

        // 生成 JSON `对象` 字符串
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

        // 转义 `字符串` 中的特殊字符
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

        int indentWidth_; // 缩进宽度
    };

} // namespace bre
#endif // JSON_GENERATOR_HPP
