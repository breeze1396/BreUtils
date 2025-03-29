#pragma once

#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <string>
#include <vector>

class ECC {
public:
    ECC();
    ~ECC();

    bool generateKeyPair();
    std::string getPublicKey() const;
    std::string getPrivateKey() const;
    
    std::vector<uint8_t> GenerateSharedSecret(const std::string &peerPublicKey) const;

private:
    EVP_PKEY_CTX *pctx;
    EVP_PKEY *pkey;
};

ECC::ECC() : pctx(nullptr), pkey(nullptr) {
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
}

ECC::~ECC() {
    if (pctx) {
        EVP_PKEY_CTX_free(pctx);
        pctx = nullptr;
    }
    if (pkey) {
        EVP_PKEY_free(pkey);
        pkey = nullptr;
    }
    EVP_cleanup();
    ERR_free_strings();
}

bool ECC::generateKeyPair() {
    pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (!pctx) {
        return false;
    }
    if (EVP_PKEY_keygen_init(pctx) <= 0) {
        EVP_PKEY_CTX_free(pctx);
        pctx = nullptr;
        return false;
    }
    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_X9_62_prime256v1) <= 0) {
        EVP_PKEY_CTX_free(pctx);
        pctx = nullptr;
        return false;
    }
    if (EVP_PKEY_keygen(pctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(pctx);
        pctx = nullptr;
        return false;
    }
    return true;
}

std::string ECC::getPublicKey() const {
    if (!pkey) {
        return "";
    }
    BIO *bio = BIO_new(BIO_s_mem());
    if (!bio) {
        return "";
    }
    PEM_write_bio_PUBKEY(bio, pkey);
    char *pubKeyData;
    long pubKeyLen = BIO_get_mem_data(bio, &pubKeyData);
    std::string pubKey(pubKeyData, pubKeyLen);
    BIO_free(bio);
    return pubKey;
}

std::string ECC::getPrivateKey() const {
    if (!pkey) {
        return "";
    }
    BIO *bio = BIO_new(BIO_s_mem());
    if (!bio) {
        return "";
    }
    PEM_write_bio_PrivateKey(bio, pkey, nullptr, nullptr, 0, nullptr, nullptr);
    char *privKeyData;
    long privKeyLen = BIO_get_mem_data(bio, &privKeyData);
    std::string privKey(privKeyData, privKeyLen);
    BIO_free(bio);
    return privKey;
}

std::vector<uint8_t> ECC::GenerateSharedSecret(const std::string &peerPublicKey) const {
    std::vector<uint8_t> sharedSecret;
    if (!pkey) {
        return sharedSecret;
    }

    BIO *bio = BIO_new_mem_buf(peerPublicKey.data(), peerPublicKey.size());
    if (!bio) {
        return sharedSecret;
    }

    EVP_PKEY *peerPkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    if (!peerPkey) {
        return sharedSecret;
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx) {
        EVP_PKEY_free(peerPkey);
        return sharedSecret;
    }

    if (EVP_PKEY_derive_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(peerPkey);
        return sharedSecret;
    }

    if (EVP_PKEY_derive_set_peer(ctx, peerPkey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(peerPkey);
        return sharedSecret;
    }

    size_t secretLen;
    if (EVP_PKEY_derive(ctx, nullptr, &secretLen) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(peerPkey);
        return sharedSecret;
    }

    sharedSecret.resize(secretLen);
    if (EVP_PKEY_derive(ctx, sharedSecret.data(), &secretLen) <= 0) {
        sharedSecret.clear();
    }

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(peerPkey);
    return sharedSecret;
}
