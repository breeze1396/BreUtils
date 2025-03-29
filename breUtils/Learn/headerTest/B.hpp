#pragma once
#include <iostream>

class A;
class C;

class B {
public:
    B(){
        std::cout << "A constructor" << std::endl;
    }
    ~B(){
        std::cout << "A destructor" << std::endl;
    }
    void printA();

    void printB(){
        std::cout << "B print" << std::endl;
    }

    void printC();


    A* a;
    C* c;
};