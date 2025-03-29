#pragma once
#include <fstream>
#include <map>
#include <iostream>
#include <sstream>

/*
第一次调用IniConfigLoad::Inst()时，会加载配置文件，之后的调用都会返回同一个实例
*/

namespace bre {

struct iniSectionInfo {
    iniSectionInfo() {}
    ~iniSectionInfo() {
        _section_datas.clear();
    }

    iniSectionInfo(const iniSectionInfo& src) {
        _section_datas = src._section_datas;
    }

    iniSectionInfo& operator=(const iniSectionInfo& src) {
        if (this == &src) {
            return *this;
        }

        this->_section_datas = src._section_datas;
        return *this;
    }

    std::map<std::string, std::string> _section_datas;

    std::string operator[](const std::string& key) {
        if (_section_datas.find(key) == _section_datas.end()) {
            return "";
        }
        return _section_datas[key];
    }

    std::string GetValue(const std::string& key) {
        if (_section_datas.find(key) == _section_datas.end()) {
            return "";
        }
        return _section_datas[key];
    }
};

class IniConfigLoad {
public:
    ~IniConfigLoad() {
        _config_map.clear();
    }

    iniSectionInfo operator[](const std::string& section) {
        if (_config_map.find(section) == _config_map.end()) {
            return iniSectionInfo();
        }
        return _config_map[section];
    }

    IniConfigLoad& operator=(const IniConfigLoad& src) {
        if (&src == this) {
            return *this;
        }

        this->_config_map = src._config_map;
        return *this;
    }

    IniConfigLoad(const IniConfigLoad& src) {
        this->_config_map = src._config_map;
    }

    static IniConfigLoad& Inst(std::string filePath) {
        std::once_flag flag;
        static IniConfigLoad instance;
        std::call_once(flag, [&]() {
            instance.loadConfig(filePath);
        });
        return instance;
    }

    std::string GetValue(const std::string& section, const std::string& key) {
        if (_config_map.find(section) == _config_map.end()) {
            return "";
        }
        return _config_map[section].GetValue(key);
    }

    

private:
    IniConfigLoad() { }

    bool loadConfig(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }

        std::string line;
        std::string current_section;
        while (std::getline(file, line)) {
            // Remove comments
            size_t comment_pos = line.find(';');
            if (comment_pos != std::string::npos) {
                line = line.substr(0, comment_pos);
            }

            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\n\r"));
            line.erase(line.find_last_not_of(" \t\n\r") + 1);

            if (line.empty()) {
                continue;
            }

            if (line.front() == '[' && line.back() == ']') {
                current_section = line.substr(1, line.size() - 2);
            } else {
                size_t equal_pos = line.find('=');
                if (equal_pos != std::string::npos) {
                    std::string key = line.substr(0, equal_pos);
                    std::string value = line.substr(equal_pos + 1);

                    // Trim whitespace
                    key.erase(0, key.find_first_not_of(" \t\n\r"));
                    key.erase(key.find_last_not_of(" \t\n\r") + 1);
                    value.erase(0, value.find_first_not_of(" \t\n\r"));
                    value.erase(value.find_last_not_of(" \t\n\r") + 1);

                    _config_map[current_section]._section_datas[key] = value;
                }
            }
        }

        file.close();
        return true;
    }


    std::map<std::string, iniSectionInfo> _config_map;
};

} // namespace bre