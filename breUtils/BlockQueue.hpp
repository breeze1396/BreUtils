#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>

namespace bre {

template<class T>
class BlockQueue {
public:
    explicit BlockQueue(size_t MaxCapacity = 1024) :capacity(MaxCapacity) {
        isClose = false;
    }

    ~BlockQueue() {
        Close();
    }

    void Clear()  {
        std::lock_guard<std::mutex> locker(mtx);
        queue.clear();
    }

    bool Empty() {
        std::lock_guard<std::mutex> locker(mtx);
        return queue.empty();
    }


    bool Full() {
        std::lock_guard<std::mutex> locker(mtx);
        return queue.size() >= capacity;
    }

    void Close() {
        {   
            std::lock_guard<std::mutex> locker(mtx);
            while (!queue.empty()) queue.pop();
            isClose = true;
        }
        condProducer.notify_all();
        condConsumer.notify_all();
    }

    size_t Size() {
        std::lock_guard<std::mutex> locker(mtx);
        return queue.size();
    }


    size_t Capacity() {
        std::lock_guard<std::mutex> locker(mtx);
        return capacity;
    }

    T Back() {
        std::lock_guard<std::mutex> locker(mtx);
        return queue.back();
    }

    void Push(const T &item) {
        std::unique_lock<std::mutex> locker(mtx);
        while(queue.size() >= capacity) {       // 防止虚假唤醒
            condProducer.wait(locker);
        }
        queue.push(item);
        condConsumer.notify_one();
    }
    void Push(const T &&item) {
        std::unique_lock<std::mutex> locker(mtx);
        while(queue.size() >= capacity) {       // 防止虚假唤醒
            condProducer.wait(locker);
        }
        queue.push(item);
        condConsumer.notify_one();
    }

    bool Pop(T &item)  {
        std::unique_lock<std::mutex> locker(mtx);
        while(queue.empty()){                   // 防止虚假唤醒
            condConsumer.wait(locker);
            if(isClose){
                return false;
            }
        }
        item = queue.front();
        queue.pop();
        condProducer.notify_one();
        return true;
    }

    bool Pop(T &item, int timeout_ms)  {
        std::unique_lock<std::mutex> locker(mtx);
        while(queue.empty()){
            if(condConsumer.wait_for(locker, std::chrono::milliseconds(timeout_ms)) 
                    == std::cv_status::timeout){
                return false;
            }
            if(isClose){
                return false;
            }
        }
        item = queue.front();
        queue.pop();
        condProducer.notify_one();
        return true;
    }
    
    // 让读取加速
    void Flush() {
        condConsumer.notify_one();
    }

private:
    size_t capacity;
    bool isClose;
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable condConsumer;
    std::condition_variable condProducer;
};

} // namespace bre