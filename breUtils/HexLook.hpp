#ifndef HEXLOOK_HPP
#define HEXLOOK_HPP

#include <source_location>
#include <string_view> 
#include <iostream> 
#include <iomanip>

class HexLook {
public:
    HexLook(void* data, size_t size, bool HasColor = true) {
        _data = data;
        _size = size;
    }

    void PrintHead(int size, std::string_view msg = "",
                   std::source_location loc = std::source_location::current()) const {
        printMsg(loc, msg);
        printData(0, size);
        printTailLine(size);
    }

    void PrintTail(int size, std::string_view msg = "",
                   std::source_location loc = std::source_location::current()) const {
        printMsg(loc, msg);
        size_t offset = _size - size;
        printData(offset, _size);
        printTailLine(size);
    }


private:
    void printData(size_t start, size_t end) const {
        if(!_data) {
            return;
        }

        if(start > _size) {
            return;
        }

        if(end > _size) {
            end = _size;
        }

        if(_hasColor) {
            // 打印颜色为 黄色
            std::cout << "\033[33m";
        }
        
        std::cout << std::hex;
        for (size_t i = start; i < end; i++) {
            std::cout << (int)((uint8_t*)_data)[i] << " ";
        }

        if(_hasColor) {
            std::cout << "\033[0m";
        }
        std::cout << std::dec << std::endl;
    }

    static std::string onlyFileName(const std::string_view path) {
        return std::string(path.substr(path.find_last_of("/\\") + 1));
    }
    
    void printMsg(const std::source_location& loc, const std::string_view msg = "") const {
        std::cout << onlyFileName(loc.file_name()) << ":" << loc.line() << "\n"
                  << loc.function_name() << "\n";
        if(msg != "") {
            std::cout<< msg << "\n";
        }
    }
    
    void printTailLine(int size) const {
        size *= 2;
        if(size > 100 || size < 0) {
            size = 16;
        }
        std::string line;
        line.reserve(size);
        for(int i = 0; i < size; ++i) {
            line  += '=';
        }
        
        std::cout << line << "\n\n";
    }

    void* _data;
    size_t _size;
    bool _hasColor;
};

#endif //!HEXLOOK_HPP