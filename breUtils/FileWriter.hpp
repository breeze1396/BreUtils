#ifndef FileWriter_hpp
#define FileWriter_hpp

#include <fstream>
#include <iostream>

class FileWriter {
public:
    // ���캯��
    FileWriter(const std::string& Filename, bool is_write = false) :
        filename(Filename), m_is_write(is_write){
		if (!is_write) return;

		// ʹ�ö�����ģʽ���ļ�
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

    // д�����ݵ��ļ��ķ���
    bool WriteData(void* data, int size) {
        if (!m_is_write) {
			std::cout << "FileWriter is not write" << std::endl;
			return false;
        }

        if (data == nullptr || size <= 0) {
            return false;
        }

        // д�����ݵ��ļ�
        outFile->write(reinterpret_cast<char*>(data), size);

        // ����Ƿ�ɹ�д��
        if (!outFile->good()) {
            std::cerr << "Failed to write data to file: " << filename << std::endl;
            return false;
        }
        return true;
    }

private:
	bool m_is_write = false;
    // ʹ�ö�����ģʽ���ļ�
    std::ofstream* outFile;
    std::string filename; // �ļ���
};

#endif // FileWriter_hpp
