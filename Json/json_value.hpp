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

namespace bre {

enum class Type { Null, Bool, Int, Double, String, Array, Object };

class Value {
public:
    using Object = std::unordered_map<std::string, Value>;
    using Array = std::vector<Value>;

    Value() : type_(Type::Null), value_(std::monostate{}) {}
    Value(const Value& other) : type_(other.type_), value_(other.value_) {}
    Value(Value&& other) noexcept : type_(other.type_), value_(std::move(other.value_)) {}

    ~Value() = default;

	Value(bool b) : type_(Type::Bool), value_(b) {}
	Value(int i) : type_(Type::Int), value_(i) {}
	Value(double d) : type_(Type::Double), value_(d) {}
	Value(const std::string& s) : type_(Type::String), value_(s) {}
	Value(const Array& arr) : type_(Type::Array), value_(arr) {}
	Value(const Object& obj) : type_(Type::Object), value_(obj) {}

    Value& operator=(const Value& other) {
        if (this == &other) {
            return *this;
        }
        type_ = other.type_;
        value_ = other.value_;
    }

    Value& operator=(Value&& other) noexcept {
        if (this != &other) {
            type_ = other.type_;
            value_ = std::move(other.value_);
        }
        return *this;
    }

    Type type() const {
        return type_;
    }

    void clear() {
        type_ = Type::Null;
        value_ = std::monostate{};
    }

    void setNull() {
        clear();
    }

    void setBool(bool b) {
        type_ = Type::Bool;
        value_ = b;
    }

    void setInt(int i) {
        type_ = Type::Int;
        value_ = i;
    }

    void setDouble(double d) {
        type_ = Type::Double;
        value_ = d;
    }

    void setString(const std::string& s) {
        type_ = Type::String;
        value_ = s;
    }

    void setArray() {
        type_ = Type::Array;
        value_ = Array{};
    }

    void setObject() {
        type_ = Type::Object;
        value_ = Object{};
    }

    // 类型转换函数
    bool asBool() const {
        checkType(Type::Bool);
        return std::get<bool>(value_);
    }

    int asInt() const {
        checkType(Type::Int);
        return std::get<int>(value_);
    }

    double asDouble() const {
        checkType(Type::Double);
        return std::get<double>(value_);
    }

    std::string asString()  {
        checkType(Type::String);
        return std::get<std::string>(value_);
    }

    const std::string asString()const {
        checkType(Type::String);
        return std::get<std::string>(value_);
    }

    Array asArray() {
        checkType(Type::Array);
        return std::get<Array>(value_);
    }
    const Array asArray()const {
        checkType(Type::Array);
        return std::get<Array>(value_);
    }

    Object asObject() {
        checkType(Type::Object);
        return std::get<Object>(value_);
    }


    const Object asObject()const {
        checkType(Type::Object);
        return std::get<Object>(value_);
    }

	bool isNull() const {
		return type_ == Type::Null;
	}

	bool isBool() const {
		return type_ == Type::Bool;
	}

	bool isInt() const {
		return type_ == Type::Int;
	}

	bool isUInt() const {
		return isInt() && asInt() >= 0;
	}

	bool isIntegral() const {
		return isInt() || isUInt();
	}

	bool isDouble() const {
		return type_ == Type::Double;
	}

	bool isNumeric() const {
		return isIntegral() || isDouble();
	}

	bool isString() const {
		return type_ == Type::String;
	}

	bool isArray() const {
		return type_ == Type::Array;
	}

	bool isObject() const {
		return type_ == Type::Object;
	}

	bool isConvertibleTo(Type other) const {
		switch (other) {
		case Type::Null:
			return isNull();
		case Type::Bool:
			return isBool();
		case Type::Int:
			return isInt();
		case Type::Double:
			return isNumeric();
		case Type::String:
			return isString();
		case Type::Array:
			return isArray();
		case Type::Object:
			return isObject();
		default:
			return false;
		}
	}

    bool empty() const {
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


	void resize(int newSize) {
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
			return Value(); // 返回一个空值
        }
        return it->second;
    }

	void append(const Value& val) {
		checkType(Type::Array);
		std::get<Array>(value_).push_back(val);
	}

	void remove(size_t index) {
		checkType(Type::Array);
		Array& arr = std::get<Array>(value_);
		if (index >= arr.size()) {
			throw std::out_of_range("Array index out of range");
		}
		arr.erase(arr.begin() + index);
	}

	void remove(const std::string& key) {
		checkType(Type::Object);
		Object& obj = std::get<Object>(value_);
		obj.erase(key);
	}

	size_t size() const {
		switch (type_) {
		case Type::Array:
			return std::get<Array>(value_).size();
		case Type::Object:
			return std::get<Object>(value_).size();
		default:
			return 0;
		}
	}

    
private:
    Type type_;
    std::variant<std::monostate, bool, int, double, std::string, Array, Object> value_;


    void checkType(Type expected) const {
        if (type_ != expected) {
			throw JsonParseException::TypeError("Invalid type");
        }
    }
};


} // namespace bre
#endif // JSON_VALUE_HPP
