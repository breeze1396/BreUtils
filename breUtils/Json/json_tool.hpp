#ifndef JSON_TOOL_HPP
#define JSON_TOOL_HPP

#include "json_value.hpp"
#include "json_generator.hpp"
#include "json_exception.hpp"
#include "json_parse.hpp"

#include <string>
#include <cctype>
#include <stdexcept>
#include <sstream>

namespace bre {
namespace json {
    inline std::ostream& operator<<(std::ostream& os, const Value& val) {
        os << Generator::generate(val);
        return os;
    }

    inline std::string Value::toStyledString() const {
        return json::Generator::generate(*this, false);
    }

    inline std::string Value::toString() const {
        return json::Generator::generate(*this);
    }
} // namespace json
} // namespace JsonTool

#endif // JSON_TOOL_HPP
