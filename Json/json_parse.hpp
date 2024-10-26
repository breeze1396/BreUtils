#ifndef JSON_PARSE_HPP
#define JSON_PARSE_HPP

#include "json_value.hpp"
#include "json_exception.hpp"
#include <string>
#include <cctype>
#include <stdexcept>

#include <iostream>

namespace bre {

class Parser {
public:
    // 解析 JSON 字符串为 Value 对象
    static Value parse(const std::string& str) {
        Parser parser(str);
        return parser.parseValue();
    }

    static void parse(const std::string& str, Value& root) {
        Parser parser(str);
        root = parser.parseValue();
    }

private:
    explicit Parser(const std::string& str) : input_(str), position_(0) {}

    Value parseValue() {
        skipWhitespace();
        if (position_ >= input_.size()) {
            throw JsonParseException("Unexpected end of input");
        }

        switch (input_[position_]) {
        case 'n': return parseNull();
        case 't': case 'f': return parseBool();
        case '"': return parseString();
        case '[': return parseArray();
        case '{': return parseObject();
        default: return std::isdigit(input_[position_]) || input_[position_] == '-'
            ? parseNumber() : throw JsonParseException("Unexpected character");
        }
    }

    Value parseNull() {
        expect("null");
        return Value(); // 默认构造为 Null
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
            throw JsonParseException("Invalid boolean value");
        }
    }

    Value parseNumber() {
        size_t start = position_;
        if (input_[position_] == '-') ++position_;

        while (position_ < input_.size() && std::isdigit(input_[position_])) {
            ++position_;
        }

        if (position_ < input_.size() && input_[position_] == '.') {
            ++position_;
            while (position_ < input_.size() && std::isdigit(input_[position_])) {
                ++position_;
            }
            double value = std::stod(input_.substr(start, position_ - start));
            return Value(value);
        }

        int value = std::stoi(input_.substr(start, position_ - start));
        return Value(value);
    }

    Value parseString() {
        expect("\"");
        std::string result;

        while (position_ < input_.size() && input_[position_] != '"') {
            if (input_[position_] == '\\') {
                ++position_;
                if (position_ >= input_.size()) {
                    throw JsonParseException("Invalid escape sequence");
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
                default: throw JsonParseException("Invalid escape character");
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

    Value parseArray() {
        expect("[");
        Value arrayValue;
        arrayValue.setArray();

        skipWhitespace();
        if (input_[position_] == ']') {
            ++position_;
            return arrayValue;
        }

        while (true) {
            arrayValue.append(parseValue());
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
                throw JsonParseException("Expected ',' or ']'");
            }
        }

        return arrayValue;
    }

    Value parseObject() {
        expect("{");
        Value objectValue;
        objectValue.setObject();

        skipWhitespace();
        if (input_[position_] == '}') {
            ++position_;
            return objectValue;
        }

        while (true) {
            if (input_[position_] != '"') {
                throw JsonParseException("Expected string key");
            }
            std::string key = parseString().asString();
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
                throw JsonParseException("Expected ',' or '}'");
            }
        }

        return objectValue;
    }

    void skipWhitespace() {
        while (position_ < input_.size() && std::isspace(static_cast<unsigned char>(input_[position_]))) {
            ++position_;
        }
    }

    void expect(const std::string& expected) {
        if (input_.substr(position_, expected.size()) != expected) {
            throw JsonParseException("Expected '" + expected + "'");
        }
        position_ += expected.size();
    }

    const std::string& input_;
    size_t position_;
};

} // namespace bre
#endif // JSON_PARSE_HPP
