#pragma once

#include "forward_declarations.hpp"
#include <cstdint>
#include <string>
#include <memory>
#include <vector>

namespace bre {
    namespace big {

        class Int {
        public:
            inline Int(int64_t x = 0);
            inline Int(const Int& other);
            ~Int() = default;

            Float* ToFloat() const;  // 返回一个指向 Float 的指针
            static Int* FromFloat(const Float& f);  // 静态方法从 Float 转换为 Int

            // Other methods...
            inline Int* Add(const Int& x, const Int& y);
            inline int Cmp(const Int& y) const;
            inline Int* Div(const Int& x, const Int& y);
            inline Int* Exp(const Int& x, const Int& y, const Int* m);
            inline Int* GCD(const Int& x, const Int& y, const Int& a, const Int& b);
            inline Int* Mod(const Int& x, const Int& y);
            inline Int* Mul(const Int& x, const Int& y);
            inline Int* Neg(const Int& x);
            inline bool ProbablePrime(int n) const;
            inline Int* Set(const Int& x);
            inline Int* SetInt64(int64_t x);
            inline Int* Sqrt(const Int& x);
            inline Int* Sub(const Int& x, const Int& y);

        private:
        };

        inline Int::Int(int64_t x) {
        }

        inline Int* Int::Add(const Int& x, const Int& y) {
            return this;
        }

        inline int Int::Cmp(const Int& y) const
        {
            return 0;
        }

        inline Int* Int::Div(const Int& x, const Int& y)
        {
            return nullptr;
        }

        inline Int* Int::Exp(const Int& x, const Int& y, const Int* m)
        {
            return nullptr;
        }

        inline Int* Int::GCD(const Int& x, const Int& y, const Int& a, const Int& b)
        {
            return nullptr;
        }

        inline Int* Int::Mod(const Int& x, const Int& y)
        {
            return nullptr;
        }

        inline Int* Int::Mul(const Int& x, const Int& y)
        {
            return nullptr;
        }

        inline Int* Int::Neg(const Int& x)
        {
            return nullptr;
        }

        inline bool Int::ProbablePrime(int n) const
        {
            return false;
        }

        inline Int* Int::Set(const Int& x)
        {
            return nullptr;
        }

        inline Int* Int::SetInt64(int64_t x)
        {
            return nullptr;
        }

        inline Int* Int::Sqrt(const Int& x)
        {
            return nullptr;
        }

        inline Int* Int::Sub(const Int& x, const Int& y)
        {
            return nullptr;
        }


    } // namespace big
} // namespace bre