#pragma once
#ifndef FLAG_HPP
#define FLAG_HPP

// go flag库的c++实现
// brez 2025.01.05
// 地址：https://github.com/breeze1396/BreUtils
// 协议：MIT
/*
添加参数模板如下：
	yourProgramName -flag1=value1 -flag2 value2 -truebool=T -truebool2 -falsebool=F argc1 argc2 argc3 ...

所有指针类型，变量请自行管理周期
*/

/*
todo: 对于数字完备性的判断，比如int的范围判断
todo: 是否需要智能指针的返回？？
todo: 增加完备性测试
*/

#include <memory>
#include <string>
#include <list>
#include <map>
#include <functional>
#include <vector>
#include <iostream>
#include <sstream>
#include <cinttypes>
#include <any>

enum class FlagType {
	Bool,
	Int,
	Uint,
	Int64,
	Uint64,
	Float,
	Double,
	String
};

struct FlagInfo {
	using Shared = std::shared_ptr<FlagInfo>;

    FlagInfo(const std::string& Name, FlagType Type, const std::string& Help, void* V, const std::string& Default, bool IsRequired = false) {
		name = Name;
		type = Type;
		help = Help;
		value = V;
		defValue = Default;
		isRequired = IsRequired;
    }

    void SetVal(const std::string& str) {
        if (type == FlagType::Bool) {
			*static_cast<bool*>(value) = str == "true" ? true : false;
        }
        else if (type == FlagType::Int) {
            *static_cast<int*>(value) = std::stoi(str);
        }
        else if (type == FlagType::Uint) {
            *static_cast<unsigned int*>(value) = std::stoul(str);
        }
        else if (type == FlagType::Int64) {
            *static_cast<int64_t*>(value) = std::stoll(str);
        }
        else if (type == FlagType::Uint64) {
            *static_cast<uint64_t*>(value) = std::stoull(str);
        }
        else if (type == FlagType::Float) {
            *static_cast<float*>(value) = std::stof(str);
        }
        else if (type == FlagType::Double) {
            *static_cast<double*>(value) = std::stod(str);
        }
        else if (type == FlagType::String) {
            *static_cast<std::string*>(value) = str;
        }
    }
    void PrintInfo() const {
        // 设置cout输出true，false
		std::cout.setf(std::ios::boolalpha);
        std::cout << "name: " << name << "\n";
        switch (type)
        {
        case FlagType::Bool:
            std::cout << "\ttype: bool\n\tvalue: " << *static_cast<bool*>(value) << "\n";
            break;
        case FlagType::Int:
			std::cout << "\ttype: int\n\tvalue: " << *static_cast<int*>(value) << "\n";
            break;
        case FlagType::Uint:
			std::cout << "\ttype: uint\n\tvalue: " << *static_cast<unsigned int*>(value) << "\n";
            break;
        case FlagType::Int64:
			std::cout << "\ttype: int64\n\tvalue: " << *static_cast<int64_t*>(value) << "\n";
            break;
        case FlagType::Uint64:
			std::cout << "\ttype: uint64\n\tvalue: " << *static_cast<uint64_t*>(value) << "\n";
            break;
        case FlagType::Float:
			std::cout << "\ttype: float\n\tvalue: " << *static_cast<float*>(value) << "\n";
            break;
        case FlagType::Double:
			std::cout << "\ttype: double\n\tvalue: " << *static_cast<double*>(value) << "\n";
            break;
        case FlagType::String:
			std::cout << "\ttype: string\n\tvalue: " << *static_cast<std::string*>(value) << "\n";
            break;
        default:
            break;
        }
		std::cout << "\tdefault: " << defValue << "\n";
		std::cout << "\tisRequired: " << isRequired << "\n";
		std::cout << "\thelp: " << help << std::endl;  

        // 还原cout 输出为1, 0
        std::cout.unsetf(std::ios::boolalpha);
	}

    std::string name;
    std::string help;
	FlagType type;
    void* value = nullptr;
    std::string defValue;
    bool isRequired = false;
};

class Flag {
public:
    // 构造函数
    Flag(const std::string& program_name = "", const std::string& description = "");
    ~Flag();

    // 添加不同类型的标志方法
    void Bool(bool* pvalue, const std::string& name, bool default_value, const std::string& help);
    bool* Bool(const std::string& name, bool default_value, const std::string& help);

    void Int(int* pvalue, const std::string& name, int default_value, const std::string& help);
    int* Int(const std::string& name, int default_value, const std::string& help);
    void Uint(unsigned int* pvalue, const std::string& name, unsigned int default_value, const std::string& help);
    unsigned int* Uint(const std::string& name, unsigned int default_value, const std::string& help);
    void Int64(int64_t* pvalue, const std::string& name, int64_t default_value, const std::string& help);
    int64_t* Int64(const std::string& name, int64_t default_value, const std::string& help);
    void Uint64(uint64_t* pvalue, const std::string& name, uint64_t default_value, const std::string& help);
    uint64_t* Uint64(const std::string& name, uint64_t default_value, const std::string& help);

    void Float(float* pvalue, const std::string& name, float default_value, const std::string& help);
    void Double(double* pvalue, const std::string& name, double default_value, const std::string& help);

    void String(std::string* pvalue, const std::string& name, const std::string& default_value, const std::string& help);


	int Narg();     // 非标志参数数量
    int Nflag();    // 参数数量
	std::string Arg(size_t i); // 获取第i个非标志参数
    std::vector<std::string> Arg(); // 获取非标志参数
	std::vector<FlagInfo::Shared> VisiedAll() const { return m_flags; }
	std::string ProgramName() const { return m_program_name; }
	std::string Description() const { return m_description; }


    // 解析命令行参数
    void Parse(int argc, char** argv);
    bool Parsed() const; // 是否解析完成

    // 打印帮助信息
    void PrintDefault() const;

private:
    void addFlag(FlagInfo::Shared flag_info);

    static bool judgeSuitableValue(FlagType type ,const std::string& val);
	static bool stringToBool(const std::string& val);

private:
	std::string m_program_name;     // 程序名称
	std::string m_description;      // 程序描述
    std::string m_prefix_chars{ "-" };
    std::string m_assign_chars{ "=" };
    bool m_is_parsed = false;

    std::vector<FlagInfo::Shared> m_flags;
    std::map<std::string, FlagInfo::Shared> m_flag_map;

    std::vector<std::string> m_positional_args;      // 非标志参数?
};

Flag::Flag(const std::string& program_name, const std::string& description)
    : m_program_name(program_name), m_description(description) {}

Flag::~Flag() {}

void Flag::addFlag(FlagInfo::Shared flag_info) {
    m_flags.push_back(flag_info);
    m_flag_map[flag_info->name] = flag_info;
}

// 仅判断传入的值是否符合当前类型的预期
inline  bool Flag::judgeSuitableValue(FlagType type, const std::string& val)
{
	if (val.empty()) {
		return false;
	}

	if (type == FlagType::Bool) {
        // 1, 0, t, f, T, F, true, false, TRUE, FALSE, True, False
        if (val == "1" || val == "0" || val == "t" || val == "f" ||
            val == "T" || val == "F" || val == "true" || val == "false" ||
            val == "TRUE" || val == "FALSE" || val == "True" || val == "False") {
            return true;
        }
    }

	if (type == FlagType::Int) {
		try {
			int r = std::stoi(val);
			return true;
		}
		catch (const std::exception&) {
			return false;
		}
	}

	if (type == FlagType::Int64) {
		try {
			int r = std::stoll(val);
			return true;
		}
		catch (const std::exception&) {
			return false;
		}
	}

    if (type == FlagType::Float) {
        try {
            float r = std::stof(val);
            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }

	if (type == FlagType::Double) {
		try {
			double r = std::stod(val);
			return true;
		}
		catch (const std::exception&) {
			return false;
		}
	}

	if (type == FlagType::String) {
		return true;
	}


	// 》》》》》》   无符号类型    《《《《《《《《
	if (val[0] == '-') {
		return false;
	}
    if (type == FlagType::Uint) {
        try {
            int r = std::stoul(val);
            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }

    if (type == FlagType::Uint64) {
        try {
            int r = std::stoull(val);
            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }

    return false;
}


// 传入字符必须是1, 0, t, f, T, F, true, false, TRUE, FALSE, True, False
inline bool Flag::stringToBool(const std::string& val)
{
	if (val == "1" || val == "t" || val == "T" || val == "true" || val == "TRUE" || val == "True") {
		return true;
	} 
    else if (val == "0" || val == "f" || val == "F" || val == "false" || val == "FALSE" || val == "False") {
        return false;
    }

    return false;
}

void Flag::Bool(bool* pvalue, const std::string& name, bool default_value, const std::string& help) {
    *pvalue = default_value;
	auto flagShared = std::make_shared<FlagInfo>(name, FlagType::Bool, help, (void*)pvalue, default_value ? "true" : "false");
    addFlag(flagShared);
}

bool* Flag::Bool(const std::string& name, bool default_value, const std::string& help) {
	bool* value = new bool;
    Bool(value, name, default_value, help);
    return value;
}

void Flag::Int(int* pvalue, const std::string& name, int default_value, const std::string& help) {
    *pvalue = default_value;
    auto flagShared = std::make_shared<FlagInfo>(name, FlagType::Int, help, (void*)pvalue, std::to_string(default_value));
    addFlag(flagShared);
}

int* Flag::Int(const std::string& name, int default_value, const std::string& help) {
	int* value = new int;
    Int(value, name, default_value, help);
    return value;
}

void Flag::Uint(unsigned int* pvalue, const std::string& name, unsigned int default_value, const std::string& help) {
    *pvalue = default_value;
    auto flagShared = std::make_shared<FlagInfo>(name, FlagType::Uint, help, (void*)pvalue, std::to_string(default_value));
    addFlag(flagShared);
}

unsigned int* Flag::Uint(const std::string& name, unsigned int default_value, const std::string& help) {
	unsigned int* value = new unsigned int;
    Uint(value, name, default_value, help);
    return value;
}

void Flag::Int64(int64_t* pvalue, const std::string& name, int64_t default_value, const std::string& help) {
    *pvalue = default_value;
    auto flagShared = std::make_shared<FlagInfo>(name, FlagType::Int64, help, (void*)pvalue, std::to_string(default_value));
    addFlag(flagShared);
}

int64_t* Flag::Int64(const std::string& name, int64_t default_value, const std::string& help) {
	int64_t* value = new int64_t;
    Int64(value, name, default_value, help);
    return value;
}

void Flag::Uint64(uint64_t* pvalue, const std::string& name, uint64_t default_value, const std::string& help) {
    *pvalue = default_value;
    auto flagShared = std::make_shared<FlagInfo>(name, FlagType::Uint64, help, (void*)pvalue, std::to_string(default_value));
    addFlag(flagShared);
}

uint64_t* Flag::Uint64(const std::string& name, uint64_t default_value, const std::string& help) {
	uint64_t* value = new uint64_t;
    Uint64(value, name, default_value, help);
    return value;
}

void Flag::Float(float* pvalue, const std::string& name, float default_value, const std::string& help) {
    *pvalue = default_value;
    auto flagShared = std::make_shared<FlagInfo>(name, FlagType::Float, help, (void*)pvalue, std::to_string(default_value));
    addFlag(flagShared);
}

void Flag::Double(double* pvalue, const std::string& name, double default_value, const std::string& help) {
    *pvalue = default_value;
    auto flagShared = std::make_shared<FlagInfo>(name, FlagType::Double, help, (void*)pvalue, std::to_string(default_value));
    addFlag(flagShared);
}

void Flag::String(std::string* pvalue, const std::string& name, const std::string& default_value, const std::string& help) {
    *pvalue = default_value;
    auto flagShared = std::make_shared<FlagInfo>(name, FlagType::String, help, (void*)pvalue, default_value);
    addFlag(flagShared);
}

int Flag::Narg() {
    return static_cast<int>(m_positional_args.size());
}

int Flag::Nflag() {
    return static_cast<int>(m_flags.size());
}

std::string Flag::Arg(size_t i) {
    if (i > 0 && i <= static_cast<int>(m_positional_args.size())) {
        return m_positional_args[i - 1];
    }
    return "";
}

std::vector<std::string> Flag::Arg() {
    return m_positional_args;
}

void Flag::Parse(int argc, char** argv) {
	if (argc < 2) {
		PrintDefault();
		exit(EXIT_FAILURE);
	}

	if (m_is_parsed) {
		return;
	}

	if (m_program_name.empty()) {
		m_program_name = argv[0];
	}

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);

        if (arg.find(m_prefix_chars) != 0) { // 没有前缀，是位置参数
            m_positional_args.push_back(arg);
			continue;
        }

        size_t prefix_pos = 1;
		// 判断是否是两个前缀
		if (arg.find(m_prefix_chars+ m_prefix_chars) == 0) {
			prefix_pos = m_prefix_chars.length()*2;
		}
        size_t eq_pos = arg.find(m_assign_chars);
        std::string flag_name = arg.substr(prefix_pos, eq_pos - m_prefix_chars.length());
        auto it = m_flag_map.find(flag_name);

		// 如果没有找到flag
        if (it == m_flag_map.end()) {
            std::cerr << "Unknown flag: " << arg << std::endl;
            PrintDefault();
            exit(EXIT_FAILURE);
        }

        auto& flag = it->second;
		if (eq_pos != std::string::npos) { // 有赋值`=`号
            flag->SetVal(arg.substr(eq_pos + 1));
            continue;
        }

        std::string value;

        // 判断argc数量s
        if (i + 1 >= argc) { // 后面没有参数了
			// 如果是bool类型，可以设置为true; 其他类型，报错
			if (flag->type == FlagType::Bool) {
                flag->SetVal("true");
				continue;
			} else {
				std::cerr << "Flag " << flag_name << " needs a value" << std::endl;
				exit(EXIT_FAILURE);
			}
        }

        // 判断值是否合法
        bool isSuitVal = judgeSuitableValue(flag->type, argv[i + 1]);
        if (!isSuitVal && flag->type == FlagType::Bool) { //bool -> 判断下一个是否是参数
			if (argv[i + 1][0] == '-') { // 下一个是参数，设置为true
                flag->SetVal("true");
				continue;
			}

			std::cerr << "Bool Value needs 'T' or 'F' or 'true' or 'false' or 'TRUE' or 'FALSE' or 'True' or 'False'" << std::endl;
            exit(EXIT_FAILURE);
		} 
                    

		// 对于不合法的值，报错
        if (!isSuitVal) {
			std::cerr << "Flag " << flag_name << " needs a value" << std::endl;
			exit(EXIT_FAILURE);
		} 
                    
                    
		// 后面一定有参数，且参数符合当前类型的预期
        i++;
        if(flag->type == FlagType::Bool) {
            value = stringToBool(argv[i]) ? "true" : "false";
		}
        else {
            value = argv[i];
        }
        flag->SetVal(value);
    }
    
	// 检查是否有必须的参数没有设置
	for (const auto& flag : m_flags) {
		if (flag->isRequired && flag->value == nullptr) {
			std::cerr << "Flag " << flag->name << " is required" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
    
    m_is_parsed = true;
}

bool Flag::Parsed() const {
    return m_is_parsed;
}

void Flag::PrintDefault() const {
    std::cout << "Usage: " << m_program_name << " [flags]" << std::endl;
    std::cout << m_description << std::endl;
    std::cout << "Flags:" << std::endl;
    for (const auto& flag : m_flags) {
        std::cout << "\t-" << flag->name << "\t" << flag->help << " (default " << flag->defValue << ")" << std::endl;
    }
}

#endif // FLAG_HPP