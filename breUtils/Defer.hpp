#pragma once

#include <functional>

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
