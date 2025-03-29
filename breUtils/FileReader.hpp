#pragma once

#include <sstream>
#include <fstream>
#include <filesystem>
#include <iostream>

class FileReader {
public:
    FileReader(const std::filesystem::path& path): _readSize(0) {
        _path = path;
        _file.open(path, std::ios::binary);
        if (!_file.is_open()) {
            std::cerr << "Failed to open file: " << path << '\n';
        }

        // 获取文件大小
        _file.seekg(0, std::ios::end);
        _fileSize = _file.tellg();
        _file.seekg(0, std::ios::beg);
        
    }

    // 读取文件, 返回读取的数据，如果读取失败返回nullptr
    // size: 读取的大小
    std::unique_ptr<std::vector<uint8_t>> ReadFile(size_t size) {
        if (!_file.is_open()) {
            std::cerr << "File is not open.\n";
            return nullptr;
        }

        if(size > _fileSize - _readSize) {
            size = _fileSize - _readSize;
        }

        auto buffer = std::make_unique<std::vector<uint8_t>>(size);
        _file.read(reinterpret_cast<char*>(buffer->data()), size);
        if (_file.gcount() == 0) {
            return nullptr;
        }

        _readSize += _file.gcount();

        return buffer;
    }

    size_t GetFileSize() {
        return _fileSize;
    }

    float GetFileProgress() {
        return static_cast<float>(_readSize) / _fileSize;
    }

    bool Finished() {
        return _readSize == _fileSize;
    }

    ~FileReader() {
        if (_file.is_open()) {
            _file.close();
        }
    }
private:
    std::ifstream _file;            // 文件流
    std::filesystem::path _path;    // 文件路径

    size_t _fileSize;               // 文件大小
    size_t _readSize;               // 已读取大小
};