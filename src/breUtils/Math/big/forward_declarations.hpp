#pragma once
#pragma once

namespace bre {
    namespace big {

        class Int;
        class Float;
        class Rat;

        enum class RoundingMode {
            ToNearestEven,
            ToNearestAway,
            ToZero,
            AwayFromZero,
        };

        enum class Accuracy {
            Below,
            Exact,
            Above,
        };

    } // namespace big
} // namespace bre