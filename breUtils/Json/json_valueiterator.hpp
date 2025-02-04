#ifndef JSON_VALUEITERATOR_HPP
#define JSON_VALUEITERATOR_HPP

#include <iterator>
#include "json_value.hpp"

namespace bre {

// 定义 ValueIterator 模板类
template<typename ValueType>
class ValueIterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = ValueType;
    using difference_type = std::ptrdiff_t;
    using pointer = ValueType*;
    using reference = ValueType&;

    // 默认构造函数
    ValueIterator() = default;

    // 构造函数用于数组类型的迭代器
    ValueIterator(typename Value::Array::iterator arrayIt)
        : arrayIterator_(arrayIt), isObject_(false) {}

    // 构造函数用于对象类型的迭代器
    ValueIterator(typename Value::Object::iterator objectIt)
        : objectIterator_(objectIt), isObject_(true) {}

    // 解引用运算符，用于访问值
    reference operator*() {
        return isObject_ ? objectIterator_->second : *arrayIterator_;
    }

    // 成员访问运算符
    pointer operator->() {
        return &(operator*());
    }

    // 前置递增运算符
    ValueIterator& operator++() {
        if (isObject_) {
            ++objectIterator_;
        }
        else {
            ++arrayIterator_;
        }
        return *this;
    }

    // 后置递增运算符
    ValueIterator operator++(int) {
        ValueIterator temp = *this;
        ++(*this);
        return temp;
    }

    // 前置递减运算符
    ValueIterator& operator--() {
        if (isObject_) {
            --objectIterator_;
        }
        else {
            --arrayIterator_;
        }
        return *this;
    }

    // 后置递减运算符
    ValueIterator operator--(int) {
        ValueIterator temp = *this;
        --(*this);
        return temp;
    }

    // 相等运算符
    bool operator==(const ValueIterator& other) const {
        return isObject_ ? objectIterator_ == other.objectIterator_
            : arrayIterator_ == other.arrayIterator_;
    }

    // 不相等运算符
    bool operator!=(const ValueIterator& other) const {
        return !(*this == other);
    }

private:
    typename Value::Array::iterator arrayIterator_;
    typename Value::Object::iterator objectIterator_;
    bool isObject_ = false; // 标识当前迭代器是否用于对象
};

// 定义常量迭代器
template<typename ValueType>
class ConstValueIterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = const ValueType;
    using difference_type = std::ptrdiff_t;
    using pointer = const ValueType*;
    using reference = const ValueType&;

    // 默认构造函数
    ConstValueIterator() = default;

    // 构造函数用于数组类型的常量迭代器
    ConstValueIterator(typename Value::Array::const_iterator arrayIt)
        : arrayIterator_(arrayIt), isObject_(false) {}

    // 构造函数用于对象类型的常量迭代器
    ConstValueIterator(typename Value::Object::const_iterator objectIt)
        : objectIterator_(objectIt), isObject_(true) {}

    // 解引用运算符，用于访问值
    reference operator*() const {
        return isObject_ ? objectIterator_->second : *arrayIterator_;
    }

    // 成员访问运算符
    pointer operator->() const {
        return &(operator*());
    }

    // 前置递增运算符
    ConstValueIterator& operator++() {
        if (isObject_) {
            ++objectIterator_;
        }
        else {
            ++arrayIterator_;
        }
        return *this;
    }

    // 后置递增运算符
    ConstValueIterator operator++(int) {
        ConstValueIterator temp = *this;
        ++(*this);
        return temp;
    }

    // 前置递减运算符
    ConstValueIterator& operator--() {
        if (isObject_) {
            --objectIterator_;
        }
        else {
            --arrayIterator_;
        }
        return *this;
    }

    // 后置递减运算符
    ConstValueIterator operator--(int) {
        ConstValueIterator temp = *this;
        --(*this);
        return temp;
    }

    // 相等运算符
    bool operator==(const ConstValueIterator& other) const {
        return isObject_ ? objectIterator_ == other.objectIterator_
            : arrayIterator_ == other.arrayIterator_;
    }

    // 不相等运算符
    bool operator!=(const ConstValueIterator& other) const {
        return !(*this == other);
    }

private:
    typename Value::Array::const_iterator arrayIterator_;
    typename Value::Object::const_iterator objectIterator_;
    bool isObject_ = false; // 标识当前迭代器是否用于对象
};


}
#endif // JSON_VALUEITERATOR_HPP
