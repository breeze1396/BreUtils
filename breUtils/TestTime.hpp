#ifndef TestTime_h
#define TestTime_h

#include "../Singleton.hpp"

#include <functional>
#include <chrono>
#include <iostream>
#include <string_view>
#include <fstream>
#include <source_location>
#include <map>

/*
1.用于找到代码整体运行哪一个部分耗时
使用方式：
Hit：使用单例的时候，每次在需要测试的地方Hit，这一个Hit将会和上一个Hit之间的时间差打印出来,
    如果设置需要设置线性，即所有的Hit次数应该是一致的
原理： 比如在A.cpp B.cpp C.cpp D.cpp中都有一个Hit
    由于多线程的原因, 所以可能会出现 A的hit运行两次，B的hit才运行，
    实际运行A->A->B, 所以本来是第一个A和B之间的时间差，但是实际上是第二个A和B之间的时间差
    所以增加map记录是否存在和次数，List记录Hit应该的顺序

Inteval：使用单例的时候，每次在需要测试的地方Inteval，对于这一个Inteval两次的时间差打印出来

2.静态函数mesture测试当前代码块的耗时
使用方式：TestTime::mesture([]{
    // code block
}, true, "code block name");
*/


class TestTime : public Singleton<TestTime>
{
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

    void Hit(std::string msg, bool is_write_file = false, std::source_location loc = std::source_location::current()) {        
        
    }

    void SetFile(std::string file_name = "TestTime.txt") {
        m_file = new std::ofstream(file_name);
        if(!m_file->is_open()) {
            std::cerr << "Open file failed\n";
            return;
        }
        m_is_write_file = true;
    }

    void Inteval(std::string msg, std::source_location loc = std::source_location::current()) {
        // 先查看map中是否有这个msg, 如果有说明是第二次，打印时间差，如果没有说明是第一次，记录时间
        auto key = loc.function_name();
        if(m_intevalMap.find(key) != m_intevalMap.end()) {
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::micro> elapsed = now - m_intevalMap[key];
            std::string out_msg = "Inteval: " + key + " " + msg + " Spend: " + std::to_string(elapsed.count()) + " us\n";
            std::cout << out_msg;
        } else {
            m_intevalMap[key] = std::chrono::high_resolution_clock::now();
        }
    }

private:
    TestTime() {
        m_is_write_file = false;
    }
    ~TestTime() {
        delete m_file;
    }
    TestTime(const TestTime&) = delete;
    TestTime& operator=(const TestTime&) = delete;

    friend class Singleton<TestTime>;

private:
    std::string m_lastHitMsg;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastHitTime;

    // 针对Inteval的map， key是loc， value是时间
    std::map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> m_intevalMap;

    // 针对Hit的
    // hitmap_count， key是loc，value是次数；
    // hitmap_time， key是loc，value是时间
    // hitlist，记录hit的顺序
    std::map<std::string, int> m_hitMapCount;
    std::map<std::string, std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>>> m_hitMapTime;
    std::vector<std::string> m_hitList;

    std::ofstream* m_file;
    bool m_is_write_file = false;
};


#endif /* TestTime_h */