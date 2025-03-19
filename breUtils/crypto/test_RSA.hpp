#pragma once

#include "RSA.hpp"
#include <iostream>
#include <cassert>

void test_generateKeys()
{
    Rsa rsa;
    assert(rsa.generateKeys() && "Key generation failed");
    std::cout << "test_generateKeys passed" << std::endl;
}

void test_encrypt_decrypt()
{
    Rsa rsa;
    assert(rsa.generateKeys() && "Key generation failed");

    std::string plaintext = "Hello, World!";
    std::vector<unsigned char> ciphertext = rsa.encrypt(plaintext);
    assert(!ciphertext.empty() && "Encryption failed");

    std::string decryptedtext = rsa.decrypt(ciphertext);
    assert(decryptedtext == plaintext && "Decryption failed");

    std::cout << "test_encrypt_decrypt passed" << std::endl;
}

int testRSA()
{
    test_generateKeys();
    test_encrypt_decrypt();

    std::cout << "All tests passed" << std::endl;
    return 0;
}