#ifndef BRE_HEXCOV_HPP
#define BRE_HEXCOV_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <bitset>
namespace bre {
    class HexCov {
    public:
        // ��ʮ�������ַ���ת��Ϊ��ͨ�ַ���
        static std::string hexToStr(const std::string& hex) {
            if (hex.length() % 2 != 0) {
                return "";
            }

            std::string result;
            result.reserve(hex.length() / 2);

            for (std::size_t i = 0; i < hex.length(); i += 2) {
                std::string byteString = hex.substr(i, 2);
                char byte = static_cast<char>(std::stoi(byteString, nullptr, 16));
                result.push_back(byte);
            }

            return result;
        }

        // ��ʮ�������ַ���ת��Ϊ����
        static int hexToInt(const std::string& hex) {
            int result = 0;
            std::istringstream(hex) >> std::hex >> result;
            return result;
        }

        // ����ͨ�ַ���ת��Ϊʮ�������ַ���
        static std::string strToHex(const std::string& str) {
            std::ostringstream oss;
            for (unsigned char c : str) {
                oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
            }
            return oss.str();
        }

        // ������ת��Ϊʮ�����Ʊ�ʾ��Ĭ�Ϸ����ַ�������ѡ���ض�������ʽ��std::vector<bool>
        static std::string intToHex(int value, bool asBinaryVector = false, std::vector<bool>* binVec = nullptr) {
            std::ostringstream oss;
            oss << std::hex << value;
            std::string hexStr = oss.str();

            if (asBinaryVector && binVec != nullptr) {
                binVec->clear();
                for (char hexDigit : hexStr) {
                    int digit = (isdigit(hexDigit)) ? hexDigit - '0' : tolower(hexDigit) - 'a' + 10;
                    std::bitset<4> bits(digit);
                    for (int i = 3; i >= 0; --i) {
                        binVec->push_back(bits[i]);
                    }
                }
            }
            return hexStr;
        }
    };
} // bre
#endif // BRE_HEXCOV_HPP
