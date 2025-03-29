#include <iostream>
#include <cassert>
#include "Timer.hpp"

void testWait() {
    bre::Timer timer(1000); // 1 second
    auto start = std::chrono::high_resolution_clock::now();
    timer.Wait([]() {
        std::cout << "Wait function executed." << std::endl;
    });
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    assert(duration >= 1000 && "Wait function did not wait for the specified interval.");
}

void testAsyncWait() {
    bre::Timer timer(500); // 0.5 second
    bool executed = false;
    timer.AsyncWait([&executed]() {
        std::cout << "AsyncWait function executed." << std::endl;
        executed = true;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    assert(executed && "AsyncWait function did not execute.");
    timer.Cancel();
}

void testAsyncWaitLoop() {
    bre::Timer timer(200); // 0.2 second
    int count = 0;
    timer.AsyncWait([&count]() {
        std::cout << "AsyncWait loop function executed." << std::endl;
        count++;
    }, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    timer.Cancel();
    assert(count >= 4 && "AsyncWait loop function did not execute the expected number of times.");
}

void testCancel() {
    bre::Timer timer(10000); // 10 second
    bool executed = false;
    timer.AsyncWait([&executed]() {
        std::cout << "AsyncWait function executed." << std::endl;
        executed = true;
    });
    timer.Cancel();
    assert(!executed && "AsyncWait function executed after being cancelled.");
}

int testTimer() {
    testWait();
    testAsyncWait();
    testAsyncWaitLoop();
    testCancel();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}