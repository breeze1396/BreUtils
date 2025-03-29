#pragma once
#include <iostream>

class B;
class C;

class C {
public:
    C(){
        std::cout << "A constructor" << std::endl;
    }
    ~C(){
        std::cout << "A destructor" << std::endl;
    }
    void printA();

    void printB();

    void printC(){
        std::cout << "C print" << std::endl;
    }


    B* b;
    A* a;
};