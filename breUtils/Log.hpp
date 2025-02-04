#ifndef LOG_HPP
#define LOG_HPP

#include "BlockQueue.hpp"
#include "Buffer.hpp"
#include <iostream>
#include <exception>
#include <string>
#include <fstream>
#include <mutex>
#include <thread>
#include <filesystem>
#include <format>
#include <chrono>
#include <memory>
#include <source_location>


namespace bre {

namespace fs = std::filesystem;
using std::lock_guard;
using std::mutex;


enum class LogLevel { DEBUG, INFO, WARN, ERROR, NONE};

class Log {
public:
    template<typename... Args>
    static void err(const std::string& format,  Args&&... args) {
        Instance().Write(LogLevel::ERROR, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void warn(const std::string& format,  Args&&... args) {
        Instance().Write(LogLevel::WARN, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void info(const std::string& format, Args&&... args) {
        Instance().Write(LogLevel::INFO, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void debug(const std::string& format,  Args&&... args) {
        Instance().Write(LogLevel::DEBUG, format, std::forward<Args>(args)...);
    }

    static Log& Instance() {
        static Log instance;
        static std::once_flag flag;
        std::call_once(flag, [&]() {
            instance.Init(LogLevel::INFO);
        });
        return instance;
    }

    void Init(LogLevel level,  bool console = false, std::string pathStr = "./log", 
            std::string suffixStr = ".log",  int maxQueueCapacity = 1024) {
        hasConsole = console;
        isOpen = true;
        SetLevel(level);
        path = pathStr;
        suffix = suffixStr;
        // 当容量大于0时，开启异步模式 
        if(maxQueueCapacity > 0) {
            isAsync = true;
            if(!queue) {
                queue = std::make_unique<BlockQueue<std::string>>();
            }
            if(!writeThread) {
                writeThread = std::make_unique<std::thread>(FlushLogThread);
            }
        } else {
            isAsync = false;
        }


        lineCount = 0;
        today = 0;

        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm t = *std::localtime(&in_time_t);
        today = t.tm_mday;

        std::string fileName = (fs::path(path) / (std::to_string(t.tm_year + 1900) + "_" +
                                std::to_string(t.tm_mon + 1) + "_" +
                                std::to_string(t.tm_mday) + suffix)).string();
        {
            std::lock_guard<std::mutex> locker(mtx);

            buff.RetrieveAll();
            if (fileStream.is_open()) {
                Flush();
                fileStream.close();
            }

            // 判断文件是否存在
            if (!fs::exists(fileName)) {
                fs::create_directories(fs::path(fileName).parent_path()); // 确保父目录存在
                std::ofstream tempFile(fileName);   // 创建文件
                tempFile.close();
            }

            fileStream.open(fileName, std::ios::out | std::ios::app);
            if(!fileStream.is_open()) {
                throw std::runtime_error("log file can't open, fileName: "+fileName);
            }
        }
        
        #ifdef _DEBUG
        std::cout << "log init done" << std::endl;
        #endif
    }


    static void FlushLogThread() {
        Log::Instance().asyncWrite();
    }

    template<typename... Args>
    void Write(LogLevel level, const std::string& format, Args&&... args) {
        if (!isOpen || level < GetLevel()) {
            return;
        }

        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::tm t = *std::localtime(&in_time_t);

        std::string timeStr = std::format("{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}.{:06d} ",
                                        t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                                        t.tm_hour, t.tm_min, t.tm_sec,
                                        std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count() % 1000000);

        std::string formattedMessage = std::vformat(format, std::make_format_args(args...));

        {   // 日志日期 日志行数
            std::lock_guard<std::mutex> locker(mtx);
            if (today != t.tm_mday || (lineCount && (lineCount % MAX_LINES == 0))) {
                if (today != t.tm_mday) {
                    lineCount = 0;
                    today = t.tm_mday;
                }

                std::string fileName = fs::path(path) / (std::to_string(t.tm_year + 1900) + "_" +
                                                        std::to_string(t.tm_mon + 1) + "_" +
                                                        std::to_string(t.tm_mday) + suffix);
                if (today != t.tm_mday) {
                    fileName += suffix;
                } else {
                    fileName += "-" + std::to_string(lineCount / MAX_LINES) + suffix;
                }

                if (fileStream.is_open()) {
                    fileStream.close();
                }

                fileStream.open(fileName, std::ios::out | std::ios::app);
                if (!fileStream.is_open()) {
                    throw std::runtime_error("log file can't open, fileName: " + fileName);
                }
            }


            lineCount++;
            appendLogLevelTitle(level);

            std::string fullMessage = timeStr + buff.RetrieveAll() + formattedMessage + "\n";


            if (hasConsole) {
                std::cout << fullMessage;
            }
            if (isAsync && queue && !queue->Full()) {
                queue->Push(fullMessage);
            } else {
                fileStream << fullMessage;
                fileStream.flush();

            }
        }
    }

    void Flush() {
        if(isAsync) { 
            queue->Flush(); 
        }

        if (fileStream.is_open()) {
            fileStream.flush();
        }
    }

    LogLevel GetLevel() {
        lock_guard<mutex> locker(mtx);
        return level;
    }

    void SetLevel(LogLevel l)  {
        lock_guard<mutex> locker(mtx);
        level = l;
    }

    bool IsOpen() const { return isOpen; }

private:
    Log() {
        lineCount = 0;
        isAsync = false;
        writeThread = nullptr;
        queue = nullptr;
        today = 0;
    }

    void appendLogLevelTitle(LogLevel level) {
        switch(level) {
        case LogLevel::DEBUG:
            buff.Append("[debug]: ", 9);
            break;
        case LogLevel::INFO:
            buff.Append("[info] : ", 9);
            break;
        case LogLevel::WARN:
            buff.Append("[warn] : ", 9);
            break;
        case LogLevel::ERROR:
            buff.Append("[error]: ", 9);
            break;
        default:
            buff.Append("[info] : ", 9);
            break;
        }
    }

    ~Log() {
        if(writeThread && writeThread->joinable()) {
            while(!queue->Empty()) {
                queue->Flush();
            };
            queue->Close();
            writeThread->join();
        }

        if (fileStream.is_open()) {
            std::lock_guard<std::mutex> locker(mtx);
            fileStream.flush();
            fileStream.close();
        }
    }

    void asyncWrite() {
        std::string str;
        while (queue->Pop(str)) {
            lock_guard<mutex> locker(mtx);
            if (fileStream.is_open()) {
                fileStream << str;
            }
        }
    }

private:
    static const int MAX_LINES = 5000;

    std::string path;
    std::string suffix;

    int lineCount = 0;
    int today;

    bool isOpen = true;
    bool hasConsole = true;

    Buffer buff;
    LogLevel level;
    bool isAsync;

    std::ofstream fileStream;
    std::unique_ptr<BlockQueue<std::string>> queue;
    std::unique_ptr<std::thread> writeThread;
    mutable std::mutex mtx;
};


} // namespace bre

#endif // LOG_HPP