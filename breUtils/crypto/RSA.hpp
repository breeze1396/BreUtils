#pragma once

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <string>
#include <vector>

class Rsa
{
private:
    EVP_PKEY* pkey;

public:
    Rsa();
    ~Rsa();

    bool generateKeys(int bits = 2048);
    std::vector<unsigned char> encrypt(const std::string& plaintext);
    std::string decrypt(const std::vector<unsigned char>& ciphertext);
};

Rsa::Rsa() : pkey(nullptr)
{
}

Rsa::~Rsa()
{
    if (pkey)
    {
        EVP_PKEY_free(pkey);
    }
}

bool Rsa::generateKeys(int bits)
{
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx)
        return false;

    if (EVP_PKEY_keygen_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, bits) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    EVP_PKEY_CTX_free(ctx);
    return true;
}

std::vector<unsigned char> Rsa::encrypt(const std::string& plaintext)
{
    std::vector<unsigned char> ciphertext(EVP_PKEY_size(pkey));
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);

    if (!ctx)
        return {};

    if (EVP_PKEY_encrypt_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        return {};
    }

    size_t outlen = ciphertext.size();
    if (EVP_PKEY_encrypt(ctx, ciphertext.data(), &outlen, reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.size()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        return {};
    }

    ciphertext.resize(outlen);
    EVP_PKEY_CTX_free(ctx);
    return ciphertext;
}

std::string Rsa::decrypt(const std::vector<unsigned char>& ciphertext)
{
    std::vector<unsigned char> plaintext(EVP_PKEY_size(pkey));
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);

    if (!ctx)
        return {};

    if (EVP_PKEY_decrypt_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        return {};
    }

    size_t outlen = plaintext.size();
    if (EVP_PKEY_decrypt(ctx, plaintext.data(), &outlen, ciphertext.data(), ciphertext.size()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        return {};
    }

    plaintext.resize(outlen);
    EVP_PKEY_CTX_free(ctx);
    return std::string(plaintext.begin(), plaintext.end());
}
