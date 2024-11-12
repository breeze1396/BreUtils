#include "Json.hpp"
#include <iostream>
#include <fstream>


using namespace std;
using namespace bre;

static void test() {
    Value jsonArray;
    jsonArray.setArray();
    jsonArray[0] = Value();
    jsonArray[0].setInt(10);
    jsonArray[1] = Value();
    jsonArray[1].setString("hello");

    std::cout << jsonArray << "\n====\n";
	for (int i = 0; i < jsonArray.size(); i++) {
		std::cout << jsonArray[i] << std::endl;
	}

    Value jsonObject;
    jsonObject.setObject();
    jsonObject["key1"] = Value();
    jsonObject["key1"].setBool(true);


}

static void test2() {
    std::string jsonString = R"({"name": "John", "age": 30, "married": false, "children": ["Anna", "Bob"]})";
    try {
        Value root;
        Parser::parse(jsonString, root);
        std::cout << root["name"].asString() << std::endl; // 输出 "John"
        std::cout << root["age"].asInt() << std::endl;     // 输出 30
        std::cout << root["children"][0].asString() << std::endl; // 输出 "Anna"
    }
    catch (const JsonParseException& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
    }
	std::cout << "\n\n";
}



void test3() {
    // 创建一个 JSON 对象
    Value root;
    root.setObject();
    root["name"] = Value("John");
    root["age"] = Value(30);
    root["married"] = Value(false);
    root["children"].setArray();
    root["children"].append(Value("Anna"));
    root["children"].append(Value("Bob"));

    // 生成 JSON 字符串
    try {
        std::string jsonString = Generator::generate(root);
        std::cout << jsonString << std::endl; // 输出生成的 JSON 字符串
    } catch (const JsonParseException& e) {
        std::cerr << "Generation error: " << e.what() << std::endl;
    }

}

void test4() {
    // 从文件中读取 JSON 字符串
    std::ifstream file("test.json");
    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return;
    }

    std::string jsonString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
	std::cout << jsonString << "\n\n===========\n\n";
    // 解析 JSON 字符串
    try {
        Value root = Parser::parse(jsonString);

        cout << root.size() << "\n";
        std::cout << root << std::endl;
    } catch (const JsonParseException& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
    }
}

int main() {
	test();
	test2();
	test3();
	test4();
	return 0;
}
