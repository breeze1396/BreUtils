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

    inline std::ostream& operator<<(std::ostream& os, const Value& val) {
        os << Generator::generate(val);
        return os;
    }

} // namespace JsonTool

#endif // JSON_TOOL_HPP
