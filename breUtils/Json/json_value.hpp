#ifndef JSON_VALUE_HPP
#define JSON_VALUE_HPP

#include "json_exception.hpp"
//#include "json_generator.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>
#include <stdexcept>
#include <limits>

namespace bre {

namespace json {
	class Generator;
    enum class Type { Null, Bool, Int, Double, String, Array, Object };

    class Value {
    public:
        using Object = std::unordered_map<std::string, Value>;
        using Array = std::vector<Value>;

        Value() : type_(Type::Null), value_(std::monostate{}) {}
        Value(const Value& other) : type_(other.type_), value_(other.value_) {}
        Value(Value&& other) noexcept : type_(other.type_), value_(std::move(other.value_)) {}

        ~Value() = default;

        explicit Value(bool b) : type_(Type::Bool), value_(b) {}
        Value(int i) : type_(Type::Int), value_(i) {}
        explicit Value(double d) : type_(Type::Double), value_(d) {}
        Value(const char* s) : type_(Type::String), value_(std::string(s)) {}
        Value(const std::string& s) : type_(Type::String), value_(s) {}
        Value(const Array& arr) : type_(Type::Array), value_(arr) {}
        Value(const Object& obj) : type_(Type::Object), value_(obj) {}

        Value& operator=(const Value& other) {
            if (this == &other) {
                return *this;
            }
            type_ = other.type_;
            value_ = other.value_;
            return *this;
        }

        Value& operator=(Value&& other) noexcept {
            if (this != &other) {
                type_ = other.type_;
                value_ = std::move(other.value_);
            }
            return *this;
        }

        // 重载比较运算符
        bool operator==(const Value& other) const;

        Type type() const {
            return type_;
        }

        void Clear() {
            type_ = Type::Null;
            value_ = std::monostate{};
        }

        void SetNull() {
            Clear();
        }

        void SetBool(bool b) {
            type_ = Type::Bool;
            value_ = b;
        }

        void SetInt(int i) {
            type_ = Type::Int;
            value_ = i;
        }

        void SetDouble(double d) {
            type_ = Type::Double;
            value_ = d;
        }

        void SetString(const std::string& s) {
            type_ = Type::String;
            value_ = s;
        }

        void SetArray() {
            type_ = Type::Array;
            value_ = Array{};
        }

        void SetObject() {
            type_ = Type::Object;
            value_ = Object{};
        }

        // 类型转换函数
        bool AsBool() const {
            checkType(Type::Bool);
            return std::get<bool>(value_);
        }

        int AsInt() const {
            checkType(Type::Int);
            return std::get<int64_t>(value_);
        }

        double AsDouble() const {
            checkType(Type::Double);
            return std::get<double>(value_);
        }

        std::string AsString() {
            checkType(Type::String);
            return std::get<std::string>(value_);
        }

        const std::string AsString()const {
            checkType(Type::String);
            return std::get<std::string>(value_);
        }

        Array AsArray() {
            checkType(Type::Array);
            return std::get<Array>(value_);
        }
        const Array AsArray()const {
            checkType(Type::Array);
            return std::get<Array>(value_);
        }

        Object AsObject() {
            checkType(Type::Object);
            return std::get<Object>(value_);
        }


        const Object AsObject()const {
            checkType(Type::Object);
            return std::get<Object>(value_);
        }

        bool IsNull() const {
            return type_ == Type::Null;
        }

        bool IsBool() const {
            return type_ == Type::Bool;
        }

        bool IsInt() const {
            return type_ == Type::Int;
        }

        bool IsUInt() const {
            return IsInt() && AsInt() >= 0;
        }

        bool IsIntegral() const {
            return IsInt() || IsUInt();
        }

        bool IsDouble() const {
            return type_ == Type::Double;
        }

        bool IsNumeric() const {
            return IsIntegral() || IsDouble();
        }

        bool IsString() const {
            return type_ == Type::String;
        }

        bool IsArray() const {
            return type_ == Type::Array;
        }

        bool IsObject() const {
            return type_ == Type::Object;
        }

        bool IsConvertibleTo(Type other) const {
            switch (other) {
            case Type::Null:
                return IsNull();
            case Type::Bool:
                return IsBool();
            case Type::Int:
                return IsInt();
            case Type::Double:
                return IsNumeric();
            case Type::String:
                return IsString();
            case Type::Array:
                return IsArray();
            case Type::Object:
                return IsObject();
            default:
                return false;
            }
        }

        bool Empty() const {
            switch (type_) {
            case Type::Null:
                return true;
            case Type::Bool:
                return false;
            case Type::Int:
                return false;
            case Type::Double:
                return false;
            case Type::String:
                return std::get<std::string>(value_).empty();
            case Type::Array:
                return std::get<Array>(value_).empty();
            case Type::Object:
                return std::get<Object>(value_).empty();
            default:
                return true;
            }
        }


        void Resize(int newSize) {
            checkType(Type::Array);
            std::get<Array>(value_).resize(newSize);
        }

        // 数组和对象访问
        Value& operator[](size_t index) {
            checkType(Type::Array);
            Array& arr = std::get<Array>(value_);
            if (index >= arr.size()) {
                arr.resize(index + 1);
            }
            return arr[index];
        }

        Value& operator[](const std::string& key) {
            if (type_ == Type::Null) {
				return (*this = Object{ {key, Value()} })[key];
            }
            checkType(Type::Object);
            return std::get<Object>(value_)[key];
        }

        const Value& operator[](size_t index) const {
            checkType(Type::Array);
            const Array& arr = std::get<Array>(value_);
            if (index >= arr.size()) {
                throw std::out_of_range("Array index out of range");
            }
            return arr[index];
        }

        const Value& operator[](const std::string& key) const {
            checkType(Type::Object);
            const Object& obj = std::get<Object>(value_);
            auto it = obj.find(key);
            if (it == obj.end()) {
                static const Value emptyValue;
                return emptyValue; // 返回一个空值
            }
            return it->second;
        }

        void Append(const Value& val) {
            checkType(Type::Array);
            std::get<Array>(value_).push_back(val);
        }

        void Remove(size_t index) {
            checkType(Type::Array);
            Array& arr = std::get<Array>(value_);
            if (index >= arr.size()) {
                throw std::out_of_range("Array index out of range");
            }
            arr.erase(arr.begin() + index);
        }

        void Remove(const std::string& key) {
            checkType(Type::Object);
            Object& obj = std::get<Object>(value_);
            obj.erase(key);
        }

        size_t Size() const {
            switch (type_) {
            case Type::Array:
                return std::get<Array>(value_).size();
            case Type::Object:
                return std::get<Object>(value_).size();
            default:
                return 0;
            }
        }

        std::string ToStyledString() const;

        std::string ToString() const;


    private:
        Type type_;
        std::variant<std::monostate, bool, int64_t, double, std::string, Array, Object> value_;


        void checkType(Type expected) const {
            if (type_ != expected) {
                throw JsonParseException::TypeError("Invalid type");
            }
        }
    };




} // namespace json
} // namespace bre
#endif // JSON_VALUE_HPP
