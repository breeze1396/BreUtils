#pragma once

#include <functional>
namespace bre{
class Defer {
public:
    template<typename Func>
    Defer(Func&& func) {
        f = std::forward<Func>(func);
    }
    ~Defer(){
        f();
    }
private:
    std::function<void()> f;
};

} // namespace bre
