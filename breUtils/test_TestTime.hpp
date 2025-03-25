#ifndef test_TestTime_h
#define test_TestTime_h

#include "TestTime.hpp"
#include "BlockQueue.hpp"
#include <thread>
#include <chrono>

void test_mesture() {
    TestTime::mesture([]{
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }, true, "Test mesture");
}

void test_Hit() {
    auto testTime = TestTime::GetInstance();
    testTime->HitSetTimeUnit("s");
    testTime->Hit("Start");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    testTime->Hit("Middle");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    testTime->Hit("End");
}

bre::BlockQueue<int> g_queue(10);
void processThread(){
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::srand(std::time(nullptr));
    auto testTime = TestTime::GetInstance();
    testTime->HitSetTimeUnit("ms");
    testTime->HitSetWriteFile("test.txt");
    testTime->HitClear();
    for(int i = 0; i < 10; i++) {
        int time_ms = std::rand() % 200 + 100;
        g_queue.Push(time_ms);
        testTime->Hit("Start");
        std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
    }
}

void comsumeThread(){
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    auto testTime = TestTime::GetInstance();
    for(int i = 0; i < 10; i++) {
        int data = 0;
        g_queue.Pop(data);
        testTime->Hit("Get data");
        std::cout << "sleep time: " << data << "ms" << std::endl;
    }
}


void test_Hit_InThread(){
    std::thread t1(processThread);
    std::thread t2(comsumeThread);
    t1.join();
    t2.join();
}

int testTestTime() {
    std::cout << "Test mesture" << std::endl;
    test_mesture();
    std::cout << "\n\nTest Hit" << std::endl;
    for(int i = 0; i < 10; i++) {
        test_Hit();
    }
    std::cout << "\n\nTest Hit In Thread" << std::endl;
    test_Hit_InThread();

    return 0;
}

#endif /* test_TestTime_h */