#pragma once

#include "TCPManager.hpp"


constexpr int PORT = 8760;

void TcpReceiverStart(){
    TCPReceiver receiver(PORT);
    int count = 0;
    while (count < 40) {
        auto data = receiver.GetStr();
        if (data.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        std::cout << "Received: " << data << std::endl;
        count++;
    }
}


void SendData(int id){
    TCPSender sender("127.0.0.1", PORT);
    int count = 0;
    while (count < 20) {
        std::string data = "Hello, world! " + std::to_string(id) + " " + std::to_string(count);
        sender.Send(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        count++;
    }    
    std::cout << "SendData " << id << " finished" << std::endl;
}

void testTCPManeger(){
    std::thread receiver(TcpReceiverStart);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::vector<std::thread> threads;
    for (int i = 0; i < 2; ++i) {
        threads.emplace_back(SendData, i);
    }
    for(auto& t: threads) {
        t.join();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    receiver.join();
}