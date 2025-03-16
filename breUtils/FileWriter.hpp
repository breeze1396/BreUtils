#ifndef FileWriter_hpp
#define FileWriter_hpp

#include <fstream>
#include <iostream>

class FileWriter {
public:
    // 构造函数
    FileWriter(const std::string& Filename, bool is_write = false) :
        filename(Filename), m_is_write(is_write){
		if (!is_write) return;

		// 使用二进制模式打开文件
		outFile = new std::ofstream(filename, std::ios::binary);

        if (!outFile->is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            throw("Failed to open file: ");
        }
    }

    ~FileWriter() {
		if (m_is_write) {
			outFile->close();
		}
		if (outFile != nullptr) {
			delete outFile;
		}

    }

    // 写入数据到文件的方法
    bool WriteData(void* data, int size) {
        if (!m_is_write) {
			std::cout << "FileWriter is not write" << std::endl;
			return false;
        }

        if (data == nullptr || size <= 0) {
            return false;
        }

        // 写入数据到文件
        outFile->write(reinterpret_cast<char*>(data), size);

        // 检查是否成功写入
        if (!outFile->good()) {
            std::cerr << "Failed to write data to file: " << filename << std::endl;
            return false;
        }
        return true;
    }

private:
	bool m_is_write = false;
    // 使用二进制模式打开文件
    std::ofstream* outFile;
    std::string filename; // 文件名
};

#endif // FileWriter_hpp
