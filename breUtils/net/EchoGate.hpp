#pragma once
#include "../crypto/RSA.hpp"
#include "../crypto/AES.hpp"
#include "TCPManager.hpp"

class EchoGate
{
public:
    EchoGate(bool is_sender, const std::string& ip, int port, int listen_port)
        : m_is_sender(is_sender) {
        m_ip = ip;
        m_port = port;
        m_listen_port = listen_port;
        m_rsa.GenerateKeys();
    }

    ~EchoGate() {
        if (m_sender) {
            delete m_sender;
        }
        if (m_receiver) {
            delete m_receiver;
        }
        m_sender = nullptr;
        m_receiver = nullptr;
    }

    using KeyAndIV = std::pair<std::vector<uint8_t>, std::vector<uint8_t>>;

    // 函数阻塞，直到AES密钥交换完成
    KeyAndIV GetAESKey() {
        std::vector<uint8_t> aes_key;
        std::vector<uint8_t> aes_iv;

        if(m_sender) {
            delete m_sender;
        }
        if(m_receiver) {
            delete m_receiver;
        }

        if (m_is_sender) {
            // Generate RSA key pair and send public key
            auto public_key = m_rsa.GetPublicKey();
            m_sender = new TCPSender(m_ip, m_port);
            m_sender->Send(public_key);
            m_receiver = new TCPReceiver(m_listen_port);

            // Receive the other party's public key
            auto en_aes_key = m_receiver->Get();
            auto en_aes_iv = m_receiver->Get();


            // Encrypt AES key with the other party's public key and send it
            aes_key = m_rsa.Decrypt(en_aes_key);
            aes_iv = m_rsa.Decrypt(en_aes_iv);

            return {aes_key, aes_iv};
        } else {
            AES aes;
            auto aes_key = aes.GetKey();
            auto aes_iv = aes.GetIV();
            m_receiver = new TCPReceiver(m_listen_port);
            auto sender_public_key = m_receiver->Get();
            m_rsa.SetPublicKey(sender_public_key);

            // Encrypt AES key with the sender's public key and send it
            auto encrypted_aes_key = m_rsa.Encrypt(aes_key);
            m_sender = new TCPSender(m_ip, m_port);
            m_sender->Send(encrypted_aes_key);

            auto encrypted_aes_iv = m_rsa.Encrypt(aes_iv);
            m_sender->Send(encrypted_aes_iv);


            return {aes_key, aes_iv};
        }
    }

private:
    bool m_is_sender = false;

    int m_port;
    int m_listen_port;
    std::string m_ip;

    bre::RSA m_rsa;
    TCPSender* m_sender{};
    TCPReceiver* m_receiver{};
};
