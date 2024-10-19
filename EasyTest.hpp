#ifndef TEST_H
#define TEST_H

#include <iostream>
#include <string>

namespace bre {

class TestFramework {
public:
    static TestFramework& getInstance() {
        static TestFramework instance;
        return instance;
    }

    void assertTrue(bool expression, const std::string& test_name, const std::string& file, int line) {
        tests_run++;
        if (!expression) {
            tests_failed++;
            std::cerr << "Test Failed: " << test_name << " (" << file << ":" << line << ")" << std::endl;
        }
    }

    template <typename T>
    void assertEqual(const T& expected, const T& actual, const std::string& test_name, const std::string& file, int line) {
        tests_run++;
        if (expected != actual) {
            tests_failed++;
            std::cerr << "Test Failed: " << test_name << " | Expected: " 
                      << expected << ", but got: " << actual 
                      << " (" << file << ":" << line << ")" << std::endl;
        }
    }

    void showResults() {
        std::cout << "\nTests run: " << tests_run << ", Tests failed: " << tests_failed << std::endl;
        if (tests_failed == 0) {
            std::cout << "All tests passed!" << std::endl;
        } else {
            std::cout << "Some tests failed." << std::endl;
        }
    }

private:
    int tests_run = 0;
    int tests_failed = 0;

    TestFramework() = default;
    TestFramework(const TestFramework&) = delete;
    TestFramework& operator=(const TestFramework&) = delete;
};

#define ASSERT_TRUE(expression) simple_test::TestFramework::getInstance().assertTrue((expression), #expression, __FILE__, __LINE__)
#define ASSERT_EQUAL(expected, actual) simple_test::TestFramework::getInstance().assertEqual((expected), (actual), #actual, __FILE__, __LINE__)
#define SHOW_RESULTS() simple_test::TestFramework::getInstance().showResults()

} // namespace simple_test

#endif // TEST_H
