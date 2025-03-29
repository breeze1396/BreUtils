#pragma once
#include "EchoGate.hpp"
#include <iostream>
#include <thread>
#include <chrono>


void startReceiver(std::vector<uint8_t>& r_aes_key, std::vector<uint8_t>& r_aes_iv) {
    EchoGate receiver(false, "127.0.0.1", 8675, 8674);

    auto [aes_key, aes_iv] = receiver.GetAESKey();
    r_aes_key = aes_key;
    r_aes_iv = aes_iv;
}

void startSender(std::vector<uint8_t>& s_aes_key, std::vector<uint8_t>& s_aes_iv) {
    EchoGate sender(true, "127.0.0.1", 8674, 8675);

    auto [aes_key, aes_iv] = sender.GetAESKey();
    s_aes_key = aes_key;
    s_aes_iv = aes_iv;
}   


int testEchoGate() {
    std::vector<uint8_t> r_aes_key;
    std::vector<uint8_t> r_aes_iv;
    std::vector<uint8_t> s_aes_key;
    std::vector<uint8_t> s_aes_iv;


    std::thread receiverThread([&](){
        startReceiver(r_aes_key, r_aes_iv);
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread senderThread([&](){
        startSender(s_aes_key, s_aes_iv);
    });

    receiverThread.join();
    senderThread.join();

    // Check if the AES keys are the same
    if (r_aes_key == s_aes_key && r_aes_iv == s_aes_iv) {
        std::cout << "EchoGate test passed." << std::endl;
    } else {
        std::cout << "EchoGate test failed." << std::endl;
    }


    return 0;
}