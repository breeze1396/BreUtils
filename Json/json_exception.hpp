#ifndef JSON_EXCEPTION_HPP
#define JSON_EXCEPTION_HPP

// JsonParseException 类，用于JSON 解析 和 生成 中的错误处理

#include <exception>
#include <source_location>
#include <string>

namespace bre {

// 定义JSON异常的类型
enum class JsonErrorType {
    SyntaxError,    // 语法错误
    TypeError,      // 类型错误（如 预期数组却得到对象）
    ValueError,     // 值错误  （如 无效的数字格式）
    KeyError,       // 键错误  （如 对象中缺失必需的键）
    UnknownError    // 未知错误
};


class JsonParseException : public std::exception {
public:
    JsonParseException(const std::string& message, JsonErrorType type = JsonErrorType::UnknownError)
        : message_(message), error_type_(type) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }

    // 返回错误类型
    JsonErrorType errorType() const noexcept {
        return error_type_;
    }

    // 静态方法：返回特定的异常对象
    static JsonParseException SyntaxError(const std::string& details, 
        const std::source_location& location = std::source_location::current()) {
		return JsonParseException("Syntax Error: " + details + " at " + locationMsg(location), JsonErrorType::SyntaxError);
    }

    static JsonParseException TypeError(const std::string& details,
        const std::source_location& location = std::source_location::current()) {
		return JsonParseException("Type Error: " + details + " at " + locationMsg(location), JsonErrorType::TypeError);
    }

    static JsonParseException ValueError(const std::string& details,
        const std::source_location& location = std::source_location::current()) {
		return JsonParseException("Value Error: " + details + " at " + locationMsg(location), JsonErrorType::ValueError);
    }

    static JsonParseException KeyError(const std::string& details,
        const std::source_location& location = std::source_location::current()) {
		return JsonParseException("Key Error: " + details + " at " + locationMsg(location), JsonErrorType::KeyError);
    }

private:
    static std::string locationMsg(std::source_location location) {
		return std::string(location.file_name()) + ":" + std::to_string(location.line()) + ": ";
    }

    std::string message_;
    JsonErrorType error_type_;
};

} // namespace bre
#endif // JSON_EXCEPTION_HPP
