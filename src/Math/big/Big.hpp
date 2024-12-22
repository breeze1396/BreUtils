#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace bre {
    namespace big {

        // Forward declarations for mutual dependencies
        class Int;
        class Float;
        class Rat;

        // Enum for RoundingMode, similar to math/big's RoundingMode in Go
        enum class RoundingMode {
            ToNearestEven,
            ToNearestAway,
            ToZero,
            AwayFromZero,
        };

        // Enum for Accuracy, similar to math/big's Accuracy in Go
        enum class Accuracy {
            Below,
            Exact,
            Above,
        };

        // Class definitions

        class Float {
        public:
            // Constructors and destructor
            Float(double x = 0.0);
            Float(const Float& other);
            ~Float() = default;

            // Static factory methods
            static std::unique_ptr<Float> ParseFloat(const std::string& s, int base, unsigned int prec, RoundingMode mode);

            // Methods
            Float* Abs(const Float& x);
            Float* Add(const Float& x, const Float& y);
            int Cmp(const Float& y) const;
            Float* Copy(const Float& x);
            Float* Mul(const Float& x, const Float& y);
            Float* Quo(const Float& x, const Float& y);
            Float* Set(const Float& x);
            Float* SetFloat64(double x);
            Float* SetPrec(unsigned int prec);
            Float* Sqrt(const Float& x);
            Float* Sub(const Float& x, const Float& y);

        private:
            // Private members (implementation details)
            // ...
        };

        class Int {
        public:
            // Constructors and destructor
            Int(int64_t x = 0);
            Int(const Int& other);
            ~Int() = default;

            // Methods
            Int* Add(const Int& x, const Int& y);
            int Cmp(const Int& y) const;
            Int* Div(const Int& x, const Int& y);
            Int* Exp(const Int& x, const Int& y, const Int* m);
            Int* GCD(const Int& x, const Int& y, const Int& a, const Int& b);
            Int* Mod(const Int& x, const Int& y);
            Int* Mul(const Int& x, const Int& y);
            Int* Neg(const Int& x);
            bool ProbablePrime(int n) const;
            Int* Set(const Int& x);
            Int* SetInt64(int64_t x);
            Int* Sqrt(const Int& x);
            Int* Sub(const Int& x, const Int& y);

        private:
            // Private members (implementation details)
            // ...
        };

        class Rat {
        public:
            // Constructors and destructor
            Rat(int64_t a = 0, int64_t b = 1);
            Rat(const Rat& other);
            ~Rat() = default;

            // Methods
            Rat* Add(const Rat& x, const Rat& y);
            int Cmp(const Rat& y) const;
            Rat* Mul(const Rat& x, const Rat& y);
            Rat* Quo(const Rat& x, const Rat& y);
            Rat* Set(const Rat& x);
            Rat* SetFrac(const Int& a, const Int& b);

        private:
            // Private members (implementation details)
            // ...
        };

    } // namespace big
} // namespace bre