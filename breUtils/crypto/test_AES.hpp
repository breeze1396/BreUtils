#pragma once

#include "AES.hpp"

#include <iostream>
#include <vector>

void test_AES_256() {
    const uint8_t key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
                                   0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                   0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                   0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};
    std::vector<uint8_t> plaintext = {'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 
                                      'a', ' ', 't', 'e', 's', 't', '.'};

    AES aes(256, key);
    std::vector<uint8_t> ciphertext = aes.Encrypt(plaintext);

    std::cout << "Ciphertext: " << aes.DecryptToStr(ciphertext) << std::endl;
    std::cout << "test_AES_256 encryption and decryption test passed!" << std::endl;
}



void test_AES_128() {
    const uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
                                   0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    std::vector<uint8_t> plaintext = {'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 
                                      'a', ' ', 't', 'e', 's', 't', '.'};
    AES aes(128, key);
    std::vector<uint8_t> ciphertext = aes.Encrypt(plaintext);
    std::cout << "Ciphertext: " << aes.DecryptToStr(ciphertext) << std::endl;
    std::cout << "test_AES_128 encryption and decryption test passed!" << std::endl;
}


void test_AES_default() {
    std::vector<uint8_t> plaintext = {'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 
                                      'a', ' ', 't', 'e', 's', 't', '.'};

    AES aes;
    aes.GetKey();
    std::vector<uint8_t> ciphertext = aes.Encrypt(plaintext);
    std::cout << "Ciphertext: " << aes.DecryptToStr(ciphertext) << std::endl;
    std::cout << "test_AES_default encryption and decryption test passed!" << std::endl;
}


int testAES() {
    test_AES_128();
    test_AES_256();
    test_AES_default();
    return 0;
}