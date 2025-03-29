#pragma once

#include "Signal.hpp"
#include <iostream>

void f() { std::cout << "free function\n"; }

struct s {
    void m() { std::cout << "member function\n"; }
    static void sm() { std::cout << "static member function\n";  }
};

struct o {
    void operator()() { std::cout << "function object\n"; }
};

void basic_member_connect() {
    s d;
    auto lambda = []() { std::cout << "lambda\n"; };
    auto lambda2 = [&]() {
        d.m();
        std::cout << "lambda2\n";
    };
    bre::Signal sig;

    sig.Connect(f);
    sig.Connect(&s::m, &d);
    sig.Disconnect(&s::m, &d);
    sig.Connect(&s::sm);
    sig.Connect(o());
    sig.Connect(lambda);
    sig.Connect(lambda2);
    sig.Emit();
}


static auto printer(std::string pos) {
    return [pos=std::move(pos)] (std::string s, int i) {
        std::cout << pos << " to print " << s << " and " << i << std::endl;
    };
}

int test2() {
    bre::Signal<std::string, int> sig;

    sig.Connect(printer("Second"), 1);
    sig.Connect(printer("Last"), 5);
    sig.Connect(printer("Third"), 2);
    sig.Connect(printer("First"), 0);
    sig.Emit("bar", 1);

    return 0;
}

class A {
public:
    void f(int i) {
        std::cout << "A::f(" << i << ")\n";
    }
};

class B {
public:
    void f(int i) {
        std::cout << "B::f(" << i << ")\n";
    }
};

void test3() {
    bre::Signal<int> sig;
    A a;
    B b;

    sig.Connect(&A::f, &a, 1);
    sig.Connect(&B::f, &b, 2);
    sig.Emit(1);

}   
