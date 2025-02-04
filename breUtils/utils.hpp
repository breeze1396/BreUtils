#ifndef BRE_UTILS_HPP
#define BRE_UTILS_HPP
#include <string>
#include <vector>
#include <functional>


namespace bre {
    class Defer {
    public:
        Defer(std::function<void()> func) : func_(func) {}
        ~Defer() { func_(); }
    private:
        std::function<void()> func_;
    };

    
}  // namespace bre

#endif  // BRE_UTILS_HPP