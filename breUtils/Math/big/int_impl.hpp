#pragma once

#include "Int.hpp"
#include "Float.hpp"

namespace bre {
    namespace big {

        Float* Int::ToFloat() const {
            auto result = new Float();
            return result;
        }

        Int* Int::FromFloat(const Float& f) {
            auto result = new Int();
            return result;
        }

    } // namespace big
} // namespace bre