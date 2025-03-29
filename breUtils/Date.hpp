#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

class Date
{
public:
    static std::string format_now_time_to_string() {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000;
    
        std::tm buf;
#if defined(__linux__) || defined(__APPLE__)
        localtime_r(&now_c, &buf); // 使用localtime_r以确保线程安全
#elif defined(_WIN32)
        localtime_s(&buf, &now_c);
#endif
        std::ostringstream formattedTime;
        formattedTime << std::put_time(&buf, "%Y-%m-%d %H:%M:%S")
                      << "." << std::setw(3) << std::setfill('0') << ms.count()
                      << "-" << std::setw(3) << std::setfill('0') << us.count();
    
        return formattedTime.str();
    }

};