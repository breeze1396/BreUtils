#ifndef TestTime_h
#define TestTime_h

#include <functional>
#include <chrono>
#include <iostream>
#include <string_view>

class TestTime {
public:
    template<typename Func>
    static void mesture(Func func, bool print = false, std::string_view sv= "") {
        if(!print) {
            func();
            return;
        }
        
        auto s = std::chrono::high_resolution_clock::now();
        func();
        auto e = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::milli> elapsed = e - s;
        std::cout << sv <<"Spend: " << elapsed.count() << " ms\n";
    }
};


#endif /* TestTime_h */