#pragma once

#include "forward_declarations.hpp"
#include <cstdint>
#include <string>
#include <memory>

namespace bre {
    namespace big {

        class Rat {
        public:
            inline Rat(int64_t a = 0, int64_t b = 1);
            inline Rat(const Rat& other);
            ~Rat() = default;

            // Methods
            inline Rat* Add(const Rat& x, const Rat& y);
            inline int Cmp(const Rat& y) const;
            inline Rat* Mul(const Rat& x, const Rat& y);
            inline Rat* Quo(const Rat& x, const Rat& y);
            inline Rat* Set(const Rat& x);
            inline Rat* SetFrac(const Int& a, const Int& b);

        private:
        };

        inline Rat::Rat(int64_t a, int64_t b) {
        }

        inline Rat* Rat::Add(const Rat& x, const Rat& y) {
            return this;
        }


    } // namespace big
} // namespace bre