#pragma once
#include <iostream>

class B;
class C;

class A {
public:
    A(){
        std::cout << "A constructor" << std::endl;
    }
    ~A(){
        std::cout << "A destructor" << std::endl;
    }
    void printA(){
        std::cout << "A print" << std::endl;
    }

    void printB();
    
    void printC();

    

    B* b;
    C* c;
};