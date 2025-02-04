#pragma once
#pragma once

#include "forward_declarations.hpp"
#include <cstdint>
#include <string>
#include <memory>
#include <cmath>
#include <limits>

namespace bre {
    namespace big {

        class Float {
        public:
            inline Float(double x = 0.0);
            inline Float(const Float& other);
            ~Float() = default;

            static inline std::unique_ptr<Float> ParseFloat(const std::string& s, int base, unsigned int prec, RoundingMode mode);

            inline Float* Abs(const Float& x);
            inline Float* Add(const Float& x, const Float& y);
            inline int Cmp(const Float& y) const;
            inline Float* Copy(const Float& x);
            inline Float* Mul(const Float& x, const Float& y);
            inline Float* Quo(const Float& x, const Float& y);
            inline Float* Set(const Float& x);
            inline Float* SetFloat64(double x);
            inline Float* SetPrec(unsigned int prec);
            inline Float* Sqrt(const Float& x);
            inline Float* Sub(const Float& x, const Float& y);

        private:
        };

        inline Float::Float(double x) {
        }

        inline Float* Float::Abs(const Float& x)
        {
            return nullptr;
        }

        inline Float* Float::Add(const Float& x, const Float& y) {
            return this;
        }

        inline int Float::Cmp(const Float& y) const
        {
            return 0;
        }

        inline Float* Float::Copy(const Float& x)
        {
            return nullptr;
        }

        inline Float* Float::Mul(const Float& x, const Float& y)
        {
            return nullptr;
        }

        inline Float* Float::Quo(const Float& x, const Float& y)
        {
            return nullptr;
        }

        inline Float* Float::Set(const Float& x)
        {
            return nullptr;
        }

        inline Float* Float::SetFloat64(double x)
        {
            return nullptr;
        }

        inline Float* Float::SetPrec(unsigned int prec)
        {
            return nullptr;
        }

        inline Float* Float::Sqrt(const Float& x)
        {
            return nullptr;
        }

        inline Float* Float::Sub(const Float& x, const Float& y)
        {
            return nullptr;
        }


    } // namespace big
} // namespace bre