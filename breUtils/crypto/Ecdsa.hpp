#pragma once


#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <vector>
#include <stdexcept>

/**
 * @class Ecdsa
 * @brief 用于椭圆曲线数字签名算法（ECDSA）操作的类。
 *
 * 此类提供了生成ECDSA密钥对、签署数据以及验证签名的功能。
 */
class Ecdsa {
public:
    Ecdsa() {
        // Create a new EC m_key pair
        m_key = EVP_PKEY_new();
        if (!m_key) {
            throw std::runtime_error("Failed to create EVP_PKEY");
        }

        EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
        if (!pctx) {
            throw std::runtime_error("Failed to create EVP_PKEY_CTX");
        }

        if (EVP_PKEY_keygen_init(pctx) <= 0) {
            EVP_PKEY_CTX_free(pctx);
            throw std::runtime_error("Failed to initialize keygen");
        }

        if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_X9_62_prime256v1) <= 0) {
            EVP_PKEY_CTX_free(pctx);
            throw std::runtime_error("Failed to set curve");
        }

        if (EVP_PKEY_keygen(pctx, &m_key) <= 0) {
            EVP_PKEY_CTX_free(pctx);
            throw std::runtime_error("Failed to generate m_key");
        }

        EVP_PKEY_CTX_free(pctx);
    }

    ~Ecdsa() {
        if (m_key) {
            EVP_PKEY_free(m_key);
        }
    }

    std::vector<uint8_t> Sign(const std::vector<uint8_t>& data) {
        EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
        if (!mdctx) throw std::runtime_error("Failed to create EVP_MD_CTX");

        if (EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, m_key) <= 0 ||
            EVP_DigestSignUpdate(mdctx, data.data(), data.size()) <= 0) {
            EVP_MD_CTX_free(mdctx);
            throw std::runtime_error("Failed to initialize or update DigestSign");
        }

        size_t siglen;
        if (EVP_DigestSignFinal(mdctx, NULL, &siglen) <= 0) {
            EVP_MD_CTX_free(mdctx);
            throw std::runtime_error("Failed to finalize DigestSign");
        }

        std::vector<uint8_t> signature(siglen);
        if (EVP_DigestSignFinal(mdctx, signature.data(), &siglen) <= 0) {
            EVP_MD_CTX_free(mdctx);
            throw std::runtime_error("Failed to finalize DigestSign");
        }

        EVP_MD_CTX_free(mdctx);
        return signature;
    }

    bool Verify(const std::vector<uint8_t>& data, const std::vector<uint8_t>& signature) {
        EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
        if (!mdctx) {
            throw std::runtime_error("Failed to create EVP_MD_CTX");
        }

        bool result = EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, m_key) == 1 &&
                      EVP_DigestVerifyUpdate(mdctx, data.data(), data.size()) == 1 &&
                      EVP_DigestVerifyFinal(mdctx, signature.data(), signature.size()) == 1;

        EVP_MD_CTX_free(mdctx);
        return result;
    }

private:
    EVP_PKEY *m_key;
};

