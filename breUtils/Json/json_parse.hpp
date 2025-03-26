#pragma once
#ifndef JSON_PARSE_HPP
#define JSON_PARSE_HPP

#include "json_value.hpp"
#include "json_exception.hpp"
#include <string>
#include <cctype>
// #include <stdexcept>
#include <iostream>
#include <charconv>

namespace bre {
namespace json {
    class Parser {
    public:
        static Value parse(const std::string& str) {
            std::string cleanStr = removeComments(removeTailComma(str));
            Parser parser(cleanStr);
            return parser.parseValue();
        }

        static void parse(const std::string& str, Value& root) {
            std::string cleanStr = removeComments(removeTailComma(str));
            Parser parser(cleanStr);
            root = parser.parseValue();
        }

    private:
        explicit Parser(const std::string& str) : input_(str), position_(0) {}

        Value parseValue() {
            skipWhitespace();
            if (position_ >= input_.size()) {
                // throw JsonParseException("Unexpected end of input");
                throwException("Unexpected end of input");
            }

            switch (input_[position_]) {
            case 'n': return parseNull();
            case 't': case 'f': return parseBool();
            case '"': return parseString();
            case '[': return parseArray();
            case '{': return parseObject();
            default:  return parseDefault();
            }
        }

        // 解析数字
        Value parseDefault() {
            bool ret = std::isdigit(input_[position_]) || input_[position_] == '-';
                
            if(!ret){
                throwException("Unexpected character");
            }

            return parseNumber();
        }

        Value parseNull() {
            expect("null");
            return Value(); // Ĭ�Ϲ���Ϊ Null
        }

        Value parseBool() {
            if (input_.substr(position_, 4) == "true") {
                position_ += 4;
                return Value(true);
            }
            else if (input_.substr(position_, 5) == "false") {
                position_ += 5;
                return Value(false);
            }
            else {
                // throw JsonParseException("Invalid boolean value");
                throwException("Invalid boolean value");
            }
        }

        Value parseNumber() {
            // 超过int， long long 的范围，直接转为 double
            // 对于含有 e 的数字，直接转为 double
            // -123456.789e-27
            size_t start = position_;
            if (input_[position_] == '-') ++position_;

            bool isDouble = false;
            while (position_ < input_.size() && std::isdigit(input_[position_])) {
                ++position_;
            }

            if (position_ < input_.size() && input_[position_] == '.') {
                isDouble = true;
                ++position_;
                while (position_ < input_.size() && std::isdigit(input_[position_])) {
                    ++position_;
                }
            }

            if (position_ < input_.size() && (input_[position_] == 'e' || input_[position_] == 'E')) {
                isDouble = true;
                ++position_;
                if (position_ < input_.size() && (input_[position_] == '+' || input_[position_] == '-')) {
                    ++position_;
                }
                while (position_ < input_.size() && std::isdigit(input_[position_])) {
                    ++position_;
                }
            }

            std::string numberStr = input_.substr(start, position_ - start);
            if (isDouble) {
                try {
                    double value = std::stod(numberStr);
                    return Value(value);
                } catch (const std::invalid_argument& e) {
                    throwException("Invalid double value");
                } catch (const std::out_of_range& e) {
                    double inf = std::numeric_limits<double>::infinity();
                    if (numberStr[0] == '-') {
                        return Value(-inf);
                    } else {
                        return Value(inf);
                    }
                }
            } else {
                int value;
                auto [ptr, ec] = std::from_chars(numberStr.data(), numberStr.data() + numberStr.size(), value);
                if (ec == std::errc()) {
                    return Value(value);
                }
                // 转换为 int 失败，尝试转换为 double
                std::cout << "parseNumber: " << numberStr << "\n";
                try {
                    double doubleValue = std::stod(numberStr);
                    return Value(doubleValue);
                } catch (const std::invalid_argument& e) {
                    throwException("Invalid number value");
                } catch (const std::out_of_range& e) {
                    double inf = std::numeric_limits<double>::infinity();
                    if (numberStr[0] == '-') {
                        return Value(-inf);
                    } else {
                        return Value(inf);
                    }
                }
            }
        }

        Value parseString() {
            expect("\"");
            std::string result;

            while (position_ < input_.size() && input_[position_] != '"') {
                if (input_[position_] == '\\') {
                    ++position_;
                    if (position_ >= input_.size()) {
                        // throw JsonParseException("Invalid escape sequence");
                        throwException("Invalid escape sequence");
                    }

                    switch (input_[position_]) {
                    case '"': result.push_back('"'); break;
                    case '\\': result.push_back('\\'); break;
                    case '/': result.push_back('/'); break;
                    case 'b': result.push_back('\b'); break;
                    case 'f': result.push_back('\f'); break;
                    case 'n': result.push_back('\n'); break;
                    case 'r': result.push_back('\r'); break;
                    case 't': result.push_back('\t'); break;
                    case 'u': ParseUnicode(result); break;
                    default: throwException("Invalid escape character");
                    }
                }
                else {
                    result.push_back(input_[position_]);
                }
                ++position_;
            }

            expect("\"");
            return Value(result);
        }

        void ParseUnicode(std::string& result) {
            position_++;
            if (position_ + 4 >= input_.size()) {
                throwException("Invalid Unicode escape sequence");
            }

            std::string hexStr = input_.substr(position_ + 1, 4);
            position_ += 4;

            unsigned int codePoint;
            std::from_chars(hexStr.data(), hexStr.data() + hexStr.size(), codePoint, 16);

            if (codePoint >= 0xD800 && codePoint <= 0xDBFF) {
                // Handle surrogate pairs
                if (position_ + 6 >= input_.size() || input_[position_] != '\\' || 
                    input_[position_ + 1] != 'u') {
                    std::cout << "parseUnicode: " << input_.substr(position_, 6) << "\n";
                    throwException("Invalid Unicode surrogate pair");
                }
                position_ += 2;
                std::string lowHexStr = input_.substr(position_, 4);
                position_ += 4;
                unsigned int lowCodePoint;
                std::from_chars(lowHexStr.data(), lowHexStr.data() + lowHexStr.size(), lowCodePoint, 16);
                if (lowCodePoint < 0xDC00 || lowCodePoint > 0xDFFF) {
                    throwException("Invalid Unicode surrogate pair");
                }
                codePoint = 0x10000 + ((codePoint - 0xD800) << 10) + (lowCodePoint - 0xDC00);
            }

            if (codePoint <= 0x7F) {
                result.push_back(static_cast<char>(codePoint));
            } else if (codePoint <= 0x7FF) {
                result.push_back(static_cast<char>(0xC0 | ((codePoint >> 6) & 0x1F)));
                result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
            } else if (codePoint <= 0xFFFF) {
                result.push_back(static_cast<char>(0xE0 | ((codePoint >> 12) & 0x0F)));
                result.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
            } else if (codePoint <= 0x10FFFF) {
                result.push_back(static_cast<char>(0xF0 | ((codePoint >> 18) & 0x07)));
                result.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
            } else {
                throwException("Invalid Unicode code point");
            }
            position_--; // 为了在循环结束后 position_++
        }

        Value parseArray() {
            expect("[");
            Value arrayValue;
            arrayValue.SetArray();

            skipWhitespace();
            if (input_[position_] == ']') {
                ++position_;
                return arrayValue;
            }

            while (true) {
                arrayValue.Append(parseValue());
                skipWhitespace();
                if (input_[position_] == ',') {
                    ++position_;
                    skipWhitespace();
                }
                else if (input_[position_] == ']') {
                    ++position_;
                    break;
                }
                else {
                    throwException("Expected ',' or ']'");
                }
            }

            return arrayValue;
        }

        Value parseObject() {
            expect("{");
            Value objectValue;
            objectValue.SetObject();

            skipWhitespace();
            if (input_[position_] == '}') {
                ++position_;
                return objectValue;
            }

            while (true) {
                if (input_[position_] != '"') {
                    // throw JsonParseException("Expected string key");
                    throwException("Expected string key");
                }
                std::string key = parseString().AsString();
                skipWhitespace();
                expect(":");
                auto value = parseValue();
                objectValue[key] = value;
                //std::cout << key << value.asString() << "\n";
                skipWhitespace();
                if (input_[position_] == ',') {
                    ++position_;
                    skipWhitespace();
                }
                else if (input_[position_] == '}') {
                    ++position_;
                    break;
                }
                else {
                    // throw JsonParseException("Expected ',' or '}'");
                    throwException("Expected ',' or '}'");
                }
            }

            return objectValue;
        }

        // 删除尾随 ","
        static std::string removeTailComma(const std::string& str) {
            /*
                [1,2,] ==> [1,2]
                {"key": "value",} ==> {"key": "value"}
            */
            std::string result;
            bool inString = false;
            bool inArray = false;
            bool inObject = false;
            for (size_t i = 0; i < str.size(); ++i) {
                if (str[i] == '"' && (i == 0 || str[i - 1] != '\\')) {
                    inString = !inString;
                }
                if (!inString) {
                    if (str[i] == '[') {
                        inArray = true;
                    } else if (str[i] == ']') {
                        inArray = false;
                    } else if (str[i] == '{') {
                        inObject = true;
                    } else if (str[i] == '}') {
                        inObject = false;
                    }
                    if ((inArray || inObject) && str[i] == ',') {
                        size_t j = i + 1;
                        while (j < str.size() && std::isspace(static_cast<unsigned char>(str[j]))) {
                            ++j;
                        }
                        if (j < str.size() && (str[j] == ']' || str[j] == '}')) {
                            continue;
                        }
                    }
                }
                result.push_back(str[i]);
            }
            return result;
        }

        // 去除注释
        static std::string removeComments(const std::string& str) {
            std::string result;
            bool inString = false;
            bool inSingleLineComment = false;
            bool inMultiLineComment = false;
        
            for (size_t i = 0; i < str.size(); ++i) {
            if (inSingleLineComment) {
                if (str[i] == '\n') {
                    inSingleLineComment = false;
                    result.push_back(str[i]);
                }
            } else if (inMultiLineComment) {
                if (str[i] == '*' && i + 1 < str.size() && str[i + 1] == '/') {
                    inMultiLineComment = false;
                    ++i;
                }
            } else {
                if (str[i] == '"' && (i == 0 || str[i - 1] != '\\' || (i >= 2 && str[i - 1] == '\\' && str[i - 2] == '\\'))) {
                    inString = !inString;
                    result.push_back(str[i]);
                } else if (!inString && str[i] == '/' && i + 1 < str.size() && str[i + 1] == '/') {
                    inSingleLineComment = true;
                    ++i;
                } else if (!inString && str[i] == '/' && i + 1 < str.size() && str[i + 1] == '*') {
                    inMultiLineComment = true;
                    ++i;
                } else {
                    result.push_back(str[i]);
                }
            }
            }
        
            return result;
        }

        void skipWhitespace() {
            while (position_ < input_.size() && std::isspace(static_cast<unsigned char>(input_[position_]))) {
                ++position_;
            }
        }

        void expect(const std::string& expected) {
            if (input_.substr(position_, expected.size()) != expected) {
                // throw JsonParseException("Expected '" + expected + "'");
                throwException("Expected '" + expected + "'");
            }
            position_ += expected.size();
        }

        [[noreturn]] void throwException(const std::string& message, std::source_location func_location = std::source_location::current()) {
            std::string error_msg = message + ": position = ";
                int msg_long = 20;
                int startIndex =  position_ > msg_long ? position_-msg_long : 0;
                int endIndex = position_ + msg_long < input_.size() ? position_ + msg_long : input_.size();
                error_msg += std::to_string(position_) + ": \'" + input_[position_] + "\' near: \""
                    + input_.substr(startIndex, position_ - startIndex) 
                    + "\033[31m" + input_[position_] + "\033[0m"        // 红色
                    + input_.substr(position_ + 1, endIndex - position_ - 1)
                    + "\"";
            throw JsonParseException(error_msg, JsonErrorType::UnknownError, func_location);
        }

        const std::string input_;
        size_t position_;
    };
} // namespace json
} // namespace bre
#endif // JSON_PARSE_HPP
