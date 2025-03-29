#pragma once
#include "iniConfigLoad.hpp"
#include <cassert>
#include <iostream>

void testLoadConfig() {
    bre::IniConfigLoad& configMgr = bre::IniConfigLoad::Inst("test.ini");

    std::string value1 = configMgr.GetValue("Section1", "Key1");
    assert(value1 == "Value1");

    std::string value2 = configMgr["Section1"]["Key2"];
    assert(value2 == "Value2");

    bre::IniConfigLoad& configMgr2 = bre::IniConfigLoad::Inst("test.ini");
    std::string value3 = configMgr2.GetValue("Section2", "Key1");
    assert(value3 == "Value3");

    std::string value4 = configMgr2.GetValue("Section2", "Key2");
    assert(value4 == "Value4");

    std::string value5 = configMgr2.GetValue("Section3", "Key1");
    assert(value5 == "");

    std::cout << "All tests passed!" << std::endl;
}

int testIniConfigLoad() {
    testLoadConfig();
    return 0;
}