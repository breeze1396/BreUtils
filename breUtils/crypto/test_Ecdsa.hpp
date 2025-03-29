#include <iostream>
#include <vector>
#include <cassert>
#include "Ecdsa.hpp"

void test_sign_and_verify() {
    Ecdsa ecdsa;

    std::vector<uint8_t> data = {'t', 'e', 's', 't', ' ', 'd', 'a', 't', 'a'};
    std::vector<uint8_t> signature = ecdsa.Sign(data);

    assert(!signature.empty() && "Signature should not be empty");

    bool is_valid = ecdsa.Verify(data, signature);
    assert(is_valid && "Signature should be valid");

    std::vector<uint8_t> invalid_data = {'i', 'n', 'v', 'a', 'l', 'i', 'd'};
    bool is_invalid = ecdsa.Verify(invalid_data, signature);
    assert(!is_invalid && "Signature should be invalid for different data");
}

int testEcdas() {
    try {
        test_sign_and_verify();
        std::cout << "All tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}