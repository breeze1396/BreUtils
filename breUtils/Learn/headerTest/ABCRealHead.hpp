/*
展示如何在头文件中使用前置声明

*/

#include "A.hpp"
#include "B.hpp"
#include "C.hpp"

#include <iostream>

void C::printA(){
    a->printA();
}

void C::printB(){
    b->printB();
}


void B::printA(){
    a->printA();
}

void B::printC(){
    c->printC();
}


void A::printB(){
    b->printB();
}

void A::printC(){
    c->printC();
}

inline void test_HeaderTest(){
    A a;
    B b;
    C c;
    a.b = &b;
    a.c = &c;
    b.a = &a;
    b.c = &c;
    c.a = &a;
    c.b = &b;
    a.printA();
    a.printB();
    a.printC();
    b.printA();
    b.printB();
    b.printC();
    c.printA();
    c.printB();
    c.printC();
}