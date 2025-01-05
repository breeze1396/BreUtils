#pragma once
/**
 * 
	NifixExpression nifix;
	string str = "5*6-5";
	int ret = nifix.calculate_expression(str);
	cout << ret << endl;
	return 0;
 */
#include <string>
#include <stack>
#include <exception>
#include <iostream>
using namespace std;
class NifixExpression
{
private:
    string str_;
protected:
    int symcmp(char a, char b) {
        if ((b == '(') ||
            ((b == '*' || b == '/') && (a == '+' || a == '-' || a == '(')) ||
            ((b == '+' || b == '-') && (a == '('))
            )
            return 1;
        else
            return 0;
    }

    void exp_transplant(const std::string& str1, std::string& str2) {
        std::stack<char> stack; 
        int j = 0;

        for (int i = 0; i < str1.size(); i++)
        {
            if (str1[i] >= '0' && str1[i] <= '9') {
                str2[j++] = str1[i];
            } else {
                if (i != 0 && str2[j - 1] != ' ')  { // 遇到数字后面的第一个运算符前加空格
                    str2[j++] = ' ';
                } if (str1[i] == ')') {     // 遇到右括号，将栈中的运算符弹出直到遇到左括号
                    while (!stack.empty() && stack.top() != '(')  {
                        str2[j++] = stack.top();
                        str2[j++] = ' ';
                        stack.pop();
                    } if (!stack.empty() && stack.top() == '(') {
                        stack.pop();        // 弹出 '('
                    }
                } else {
                    while (!stack.empty() && !symcmp(stack.top(), str1[i])) { // 当前运算符优先级小于栈顶运算符
                        str2[j++] = stack.top();
                        str2[j++] = ' ';
                        stack.pop();
                    }
                    stack.push(str1[i]);
                }
            }
        }

        if (str2[j - 1] != ' ') {
            str2[j++] = ' ';
        }

        while (!stack.empty()) { // 将栈中剩余的运算符弹出
            str2[j++] = stack.top();
            str2[j++] = ' ';
            stack.pop();
        }
    }

    int cal_s(const std::string& str) {
        std::stack<int> stack;
        int tmp = 0;

        for (int i = 0; i < str.size(); i++)
        {
            if (str[i] >= '0' && str[i] <= '9') // 计算数字
                tmp = tmp * 10 + str[i] - '0';
            else if (str[i] == ' ')     // 遇到空格，将数字压入栈中
            {
                stack.push(tmp);
                tmp = 0; // 清空 tmp
            } else { // 遇到运算符，弹出栈顶两个元素进行运算
                if (stack.size() < 2) {
                    throw std::runtime_error("Insufficient operands for operator");
                }

                int operand2 = stack.top(); stack.pop();
                int operand1 = stack.top(); stack.pop();

                switch (str[i]) {
                case '+': tmp = operand1 + operand2; break;
                case '-': tmp = operand1 - operand2; break;
                case '*': tmp = operand1 * operand2; break;
                case '/':
                    if (operand2 == 0) {
                        throw std::runtime_error("Division by zero");
                    }
                    tmp = operand1 / operand2;
                    break;
                default:
                    throw std::runtime_error("Invalid operator");
                }
            }
        }

        for (; !stack.empty(); stack.pop())
        {
            if (stack.top()) return stack.top();
        }

        return 0;
    }

public:
    int calculate_expression(const std::string& str) {
        try {
            std::string s(100, ' ');
            exp_transplant(str, s);
            int ret = cal_s(s);
            return ret;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 0;
        }
    }
};