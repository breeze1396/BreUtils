#ifndef BRE_BASE64_HPP
#define BRE_BASE64_HPP
#include <string>
namespace bre {
class Base64
{
public:
    static std::string Encode(const std::string& str) {
        const char* base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::size_t strLen = str.length();
        std::size_t len = (strLen + 2) / 3 * 4;  // 计算编码后字符串的总长度

        std::string res(len, '=');

        int i = 0, j = 0;
        for (; j + 2 < strLen; j += 3, i += 4) {
            res[i] = base64Table[(str[j] >> 2) & 0x3F];
            res[i + 1] = base64Table[((str[j] & 0x3) << 4) | ((str[j + 1] >> 4) & 0xF)];
            res[i + 2] = base64Table[((str[j + 1] & 0xF) << 2) | ((str[j + 2] >> 6) & 0x3)];
            res[i + 3] = base64Table[str[j + 2] & 0x3F];
        }

        // 处理剩余的字符
        if (j < strLen) {
            res[i] = base64Table[(str[j] >> 2) & 0x3F];
            if (j + 1 < strLen) {
                res[i + 1] = base64Table[((str[j] & 0x3) << 4) | ((str[j + 1] >> 4) & 0xF)];
                res[i + 2] = base64Table[(str[j + 1] & 0xF) << 2];
            }
            else {
                res[i + 1] = base64Table[(str[j] & 0x3) << 4];
            }
        }

        return res;
    }

    static std::string Decode(const std::string& code) {
        //根据base64表，以字符找到对应的十进制数据  
        int table[]={0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,62,0,0,0,
                63,52,53,54,55,56,57,58,
                59,60,61,0,0,0,0,0,0,0,0,
                1,2,3,4,5,6,7,8,9,10,11,12,
                13,14,15,16,17,18,19,20,21,
                22,23,24,25,0,0,0,0,0,0,26,
                27,28,29,30,31,32,33,34,35,
                36,37,38,39,40,41,42,43,44,
                45,46,47,48,49,50,51
                };  
        long len = code.length();
        long strLen;

        //计算解码后的字符串长度
        //判断编码后的字符串后是否有=   
        if(code.find("==") != std::string::npos) {   strLen=len/4*3-2;  }
        else if(code.find("=")!= std::string::npos) {strLen=len/4*3-1;  }
        else {strLen=len/4*3; } 

        std::string res(strLen,0);
    
        //以4个字符为一位进行解码
        for(int i=0, j=0; i < len-2; j+=3,i+=4) {  
            res[j]=((unsigned char)table[code[i]])<<2 | (((unsigned char)table[code[i+1]])>>4); //取出第一个字符对应base64表的十进制数的前6位与第二个字符对应base64表的十进制数的后2位进行组合  
            res[j+1]=(((unsigned char)table[code[i+1]])<<4) | (((unsigned char)table[code[i+2]])>>2); //取出第二个字符对应base64表的十进制数的后4位与第三个字符对应bas464表的十进制数的后4位进行组合  
            res[j+2]=(((unsigned char)table[code[i+2]])<<6) | ((unsigned char)table[code[i+3]]); //取出第三个字符对应base64表的十进制数的后2位与第4个字符进行组合  
        }  
        return res;  
    }
};


} // namespace bres


#endif // BRE_BASE64_HPP


