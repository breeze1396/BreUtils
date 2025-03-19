#pragma once

#include "RSA.hpp"
#include <iostream>
#include <cassert>

#include "RSA.hpp"
#include <iostream>
#include <cassert>

void TestGenerateKeys()
{
    bre::RSA rsa;
    assert(rsa.GenerateKeys());
    std::string publicKey = rsa.GetPublicKey();
    std::string privateKey = rsa.GetPrivateKey();
    assert(!publicKey.empty());
    assert(!privateKey.empty());
    std::cout << "TestGenerateKeys passed." << std::endl;
}

void TestEncryptionDecryption()
{
    bre::RSA rsa;
    assert(rsa.GenerateKeys());

    std::string plaintext = "Hello, World!";
    std::vector<unsigned char> ciphertext = rsa.Encrypt(plaintext);
    assert(!ciphertext.empty());

    std::string decryptedtext = rsa.DecryptStr(ciphertext);
    assert(decryptedtext == plaintext);
    std::cout << "TestEncryptionDecryption passed." << std::endl;
}

void TestSetGetKeys()
{
    bre::RSA rsa;
    assert(rsa.GenerateKeys());

    std::string publicKey = rsa.GetPublicKey();
    std::string privateKey = rsa.GetPrivateKey();

    bre::RSA rsa2;
    rsa2.SetPublicKey(publicKey);
    rsa2.SetPrivateKey(privateKey);

    std::string publicKey2 = rsa2.GetPublicKey();
    std::string privateKey2 = rsa2.GetPrivateKey();

    assert(publicKey == publicKey2);
    assert(privateKey == privateKey2);
    std::cout << "TestSetGetKeys passed." << std::endl;
}

int testRSA()
{
    TestGenerateKeys();
    TestEncryptionDecryption();
    TestSetGetKeys();

    std::cout << "All tests passed." << std::endl;
    return 0;
}
