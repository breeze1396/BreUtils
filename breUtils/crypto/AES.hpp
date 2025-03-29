#pragma once

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <string>
#include <vector>
class AES {
public:
    using Shared = std::shared_ptr<AES>;
    static Shared Create(int key_length_bits = 128, const uint8_t* key = nullptr) {
        return std::make_shared<AES>(key_length_bits, key);
    }

    AES(int key_length_bits = 128, const uint8_t* key = nullptr);
    ~AES();

    void SetIV(const std::vector<uint8_t>& iv) { m_iv = iv; }
    std::vector<uint8_t> GetIV() const { return m_iv; }
    void SetKey(const std::vector<uint8_t>& key) {    
        if(m_encryptCtx) { EVP_CIPHER_CTX_reset(m_encryptCtx); }
        if(m_decryptCtx) { EVP_CIPHER_CTX_reset(m_decryptCtx); }
    
        m_key = key;
        setKey();
    }
    std::vector<uint8_t> GetKey() const { return m_key; }


    std::vector<uint8_t> Encrypt(const char* plaintext, int length);
    std::vector<uint8_t> Encrypt(const std::string& plaintext);
    std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& plaintext);
    
    std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& ciphertext);
    std::string DecryptToStr(const std::vector<uint8_t>& ciphertext);

private:
    void setKey(){
        const EVP_CIPHER* cipher = nullptr;
        switch (m_key.size() * 8) {
            case 128:
                cipher = EVP_aes_128_cfb();
                break;
            case 192:
                cipher = EVP_aes_192_cfb();
                break;
            case 256:
                cipher = EVP_aes_256_cfb();
                break;
            default:
                throw std::runtime_error("Unsupported key length");
        }
        if (!EVP_EncryptInit_ex(m_encryptCtx, cipher, NULL, m_key.data(), m_iv.data())) {
            throw std::runtime_error("Failed to reinitialize encryption context with new key");
        }
        if (!EVP_DecryptInit_ex(m_decryptCtx, cipher, NULL, m_key.data(), m_iv.data())) {
            throw std::runtime_error("Failed to reinitialize decryption context with new key");
        }
    }

    EVP_CIPHER_CTX* m_encryptCtx;
    EVP_CIPHER_CTX* m_decryptCtx;
    std::vector<uint8_t> m_iv;
    std::vector<uint8_t> m_key;

    void GenerateIV();
    void GenerateKey(int key_length_bits);
};

AES::AES(int key_length_bits, const uint8_t* key) {
    m_encryptCtx = EVP_CIPHER_CTX_new();
    m_decryptCtx = EVP_CIPHER_CTX_new();

    if (!m_encryptCtx || !m_decryptCtx) {
        if (m_encryptCtx) EVP_CIPHER_CTX_free(m_encryptCtx);
        if (m_decryptCtx) EVP_CIPHER_CTX_free(m_decryptCtx);
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
    }

    if (key == nullptr) {
        GenerateKey(key_length_bits);
    } else {
        this->m_key = std::vector<uint8_t>(key, key + key_length_bits / 8);
    }

    setKey();

    GenerateIV();
}

AES::~AES() {
    EVP_CIPHER_CTX_free(m_encryptCtx);
    EVP_CIPHER_CTX_free(m_decryptCtx);
}

void AES::GenerateIV() {
    m_iv.resize(EVP_CIPHER_CTX_iv_length(m_encryptCtx));
    if (!RAND_bytes(m_iv.data(), m_iv.size())) {
        throw std::runtime_error("Failed to generate IV");
    }
}

inline void AES::GenerateKey(int key_length_bits)
{
    m_key.resize(key_length_bits / 8);
    if (!RAND_bytes(m_key.data(), m_key.size())) {
        throw std::runtime_error("Failed to generate key");
    }
}

inline std::vector<uint8_t> AES::Encrypt(const char *plaintext, int length)
{
    return Encrypt(std::vector<uint8_t>(plaintext, plaintext + length));
}

inline std::vector<uint8_t> AES::Encrypt(const std::string &plaintext)
{
    return Encrypt(std::vector<uint8_t>(plaintext.begin(), plaintext.end()));
}

std::vector<uint8_t> AES::Encrypt(const std::vector<uint8_t> &plaintext)
{
    std::vector<uint8_t> ciphertext(plaintext.size() + EVP_CIPHER_CTX_block_size(m_encryptCtx));
    int len;
    if (!EVP_EncryptInit_ex(m_encryptCtx, NULL, NULL, NULL, m_iv.data())) {
        throw std::runtime_error("Failed to initialize encryption");
    }
    if (!EVP_EncryptUpdate(m_encryptCtx, ciphertext.data(), &len, plaintext.data(), plaintext.size())) {
        throw std::runtime_error("Failed to encrypt data");
    }
    int ciphertext_len = len;
    if (!EVP_EncryptFinal_ex(m_encryptCtx, ciphertext.data() + len, &len)) {
        throw std::runtime_error("Failed to finalize encryption");
    }
    ciphertext_len += len;
    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

std::vector<uint8_t> AES::Decrypt(const std::vector<uint8_t>& ciphertext) {
    std::vector<uint8_t> plaintext(ciphertext.size());
    int len;
    if (!EVP_DecryptInit_ex(m_decryptCtx, NULL, NULL, NULL, m_iv.data())) {
        throw std::runtime_error("Failed to initialize decryption");
    }
    if (!EVP_DecryptUpdate(m_decryptCtx, plaintext.data(), &len, ciphertext.data(), ciphertext.size())) {
        throw std::runtime_error("Failed to decrypt data");
    }
    int plaintext_len = len;
    if (!EVP_DecryptFinal_ex(m_decryptCtx, plaintext.data() + len, &len)) {
        throw std::runtime_error("Failed to finalize decryption");
    }
    plaintext_len += len;
    plaintext.resize(plaintext_len);
    return plaintext;
}

inline std::string AES::DecryptToStr(const std::vector<uint8_t> &ciphertext)
{
    std::vector<uint8_t> plaintext = Decrypt(ciphertext);
    return std::string(plaintext.begin(), plaintext.end());
}
