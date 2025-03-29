#pragma once

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <string>
#include <vector>

namespace bre {

class RSA
{
private:
    EVP_PKEY* m_key;

public:
    RSA();
    ~RSA();

    bool GenerateKeys(int bits = 2048);
    
    void SetPublicKey(const std::vector<uint8_t>& publicKey){
        BIO* bio = BIO_new_mem_buf(publicKey.data(), publicKey.size());
        PEM_read_bio_PUBKEY(bio, &m_key, nullptr, nullptr);
        BIO_free(bio);
    }
    
    void SetPublicKey(const std::string& publicKey){
        BIO* bio = BIO_new_mem_buf((void*)publicKey.c_str(), -1);
        PEM_read_bio_PUBKEY(bio, &m_key, nullptr, nullptr);
        BIO_free(bio);
    }
    std::string GetPublicKey(){
        BIO* bio = BIO_new(BIO_s_mem());
        PEM_write_bio_PUBKEY(bio, m_key);
        char* buffer;
        size_t length = BIO_get_mem_data(bio, &buffer);
        std::string publicKey(buffer, length);
        BIO_free(bio);
        return publicKey;
    }

    void SetPrivateKey(const std::string& privateKey){
        BIO* bio = BIO_new_mem_buf((void*)privateKey.c_str(), -1);
        PEM_read_bio_PrivateKey(bio, &m_key, nullptr, nullptr);
        BIO_free(bio);
    }

    std::string GetPrivateKey(){
        BIO* bio = BIO_new(BIO_s_mem());
        PEM_write_bio_PrivateKey(bio, m_key, nullptr, nullptr, 0, 0, nullptr);
        char* buffer;
        size_t length = BIO_get_mem_data(bio, &buffer);
        std::string privateKey(buffer, length);
        BIO_free(bio);
        return privateKey;
    }

    std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& plaintext);
    std::vector<unsigned char> Encrypt(const std::string& plaintext);
    std::string DecryptStr(const std::vector<unsigned char>& ciphertext);
    std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& ciphertext) {
        std::vector<uint8_t> plaintext(EVP_PKEY_size(m_key));
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(m_key, nullptr);

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
        return plaintext;
    }
};

RSA::RSA() : m_key(nullptr)
{
}

RSA::~RSA()
{
    if (m_key)
    {
        EVP_PKEY_free(m_key);
    }
}

bool RSA::GenerateKeys(int bits)
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

    if (EVP_PKEY_keygen(ctx, &m_key) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    EVP_PKEY_CTX_free(ctx);
    return true;
}

std::vector<uint8_t> RSA::Encrypt(const std::vector<uint8_t> &plaintext) {
    std::vector<uint8_t> ciphertext(EVP_PKEY_size(m_key));
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(m_key, nullptr);

    if (!ctx)
        return {};

    if (EVP_PKEY_encrypt_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        return {};
    }

    size_t outlen = ciphertext.size();
    if (EVP_PKEY_encrypt(ctx, ciphertext.data(), &outlen, plaintext.data(), plaintext.size()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        return {};
    }

    ciphertext.resize(outlen);
    EVP_PKEY_CTX_free(ctx);
    return ciphertext;
}

std::vector<unsigned char> RSA::Encrypt(const std::string &plaintext)
{
    std::vector<unsigned char> ciphertext(EVP_PKEY_size(m_key));
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(m_key, nullptr);

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

std::string RSA::DecryptStr(const std::vector<unsigned char>& ciphertext)
{
    std::vector<unsigned char> plaintext(EVP_PKEY_size(m_key));
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(m_key, nullptr);

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

} // namespace bre