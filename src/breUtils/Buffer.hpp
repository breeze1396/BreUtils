#ifndef buffer_H
#define buffer_H

#include <iostream>
#include <vector>
#include <string>

namespace bre {


class Buffer {
public:
    explicit Buffer(int initBuffSize = 1024)
        : buffer(initBuffSize), readPos(0), writePos(0) 
    { }

    ~Buffer() = default;

    Buffer(const Buffer&) = delete;              // 禁止拷贝构造
    Buffer& operator=(const Buffer&) = delete;   // 禁止拷贝赋值

    // 移动构造
    Buffer(Buffer&& other) noexcept
        : buffer(std::move(other.buffer)),
        readPos(other.readPos),
        writePos(other.writePos) {
        other.readPos = 0;
        other.writePos = 0;
    }

    // 移动赋值
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            buffer = std::move(other.buffer);
            readPos = other.readPos;
            writePos = other.writePos;
            other.readPos = 0;
            other.writePos = 0;
        }
        return *this;
    }

    // 可写字节数
    size_t WritableBytes() const {
        return buffer.size() - writePos;
    }
    // 可读字节数
    size_t ReadableBytes() const {
        return writePos - readPos;
    }

    const char* Peek() const {
        return buffer.data() + readPos;
    }

    std::string Retrieve(size_t len) {
        if (len > ReadableBytes()) {
            throw std::out_of_range("Buffer::Retrieve: len is too large");
        }
        auto ret = std::string(Peek(), len);
        readPos += len;
        return ret;
    }

    std::string RetrieveUntil(const std::string end) {
        const std::size_t pos = std::string(Peek(), ReadableBytes()).find(end);
        if(pos == std::string::npos) {
            return "";
        }
        return Retrieve(pos + end.size());
    }

    void Clear() {
        readPos = writePos = 0;
    }

    std::string RetrieveAll() {
        std::string ret = std::string(Peek(), ReadableBytes());
        Clear();
        return ret;
    }

    void Append(const std::string& str) {
        Append(str.data(), str.size());
    }
    void Append(const void* data, size_t len) {
        Append(static_cast<const char*>(data), len);
    }
    void Append(const Buffer& buff) {
        Append(buff.Peek(), buff.ReadableBytes());
    }
    // 最终添加数据
    void Append(const char* str, size_t len) {
        if (WritableBytes() < len) {
            expandBuffer(len);
        }
        std::copy(str, str + len, buffer.data() + writePos);
        writePos += len;
    }

private:
    void expandBuffer(size_t len) {
        if (WritableBytes() + readPos < len) {
            // 重置缓冲区
            buffer.resize(writePos + len);
        } else {
            // 移动数据
            size_t readable = ReadableBytes();
            std::copy(buffer.data() + readPos, buffer.data() + writePos, buffer.data());
            readPos = 0;
            writePos = readPos + readable;
        }
    }
    
private:
    std::vector<char> buffer;
    size_t readPos;     // 读取偏移量
    size_t writePos;    // 写入偏移量
};


} // namespace bre
#endif // buffer_H
