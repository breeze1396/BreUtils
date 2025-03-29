#pragma once

#include "TCP.hpp"
#include "TCPManager.hpp"
#include <thread>
#include <chrono>

static void sendThread(int id, int port) {
    TCPSender sender("127.0.0.1", port);
    for(int i = 0; i < 10; i++) {
        int sleep_time = rand() % 50 + 50;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        std::string msg = "id: " + std::to_string(id) + "sleep time: " + std::to_string(sleep_time);
        sender.Send(msg);
    }
    sender.Send("Hello, world!");
    sender.Close();
}

class MyNetLogic : public NetLogic {
public:
    void OnClientConnected(const std::string& session_id) override {
        std::cout << "Server OnClientConnected: " << session_id << std::endl;
    }
    void OnClientDisconnected(const std::string& session_id) override {
        std::cout << "Server OnClientDisconnected: " << session_id << std::endl;
    }

    void handleMessage(const netMsgNode& in_msg) override {
        auto msg = in_msg;
        std::cout << "Recv msg: " << std::string(msg.Data->begin(), msg.Data->end()) << std::endl;
        std::string echo_msg = "echo: " + std::string(msg.Data->begin(), msg.Data->end());
        msg.Data->clear();
        echo_msg.insert(echo_msg.end(), msg.Data->begin(), msg.Data->end());
        msg.Data = std::make_shared<std::vector<uint8_t>>(echo_msg.begin(), echo_msg.end());
        Send(msg);
    }

};

static void test_TcpServer() {
    unsigned short port = 8764;

    NetLogic::Shared net_logic = std::make_shared<MyNetLogic>();

    TCPServer server(port, net_logic);

    std::thread server_thread([&server]() {
        server.Run();
    });

    std::vector<std::thread> send_threads;
    for(int i = 0; i < 10; i++) {
        send_threads.push_back(std::thread(sendThread, i, port));
    }

    for(auto& t: send_threads) {
        t.join();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    server.Stop();
    server_thread.join();
}


static void testClient(){
    unsigned short port = 8764;

    NetLogic::Shared net_logic = std::make_shared<MyNetLogic>();

    TCPServer server(port, net_logic);

    std::thread server_thread([&server]() {
        server.Run();
    });

    TCPClinet clinet;
    auto cb = [](const netMsgNode& msg) {
        std::cout << "clinet Recv msg: " << std::string(msg.Data->begin(), msg.Data->end()) << std::endl;
    };

    auto session = clinet.GetSession("127.0.0.1", port, cb);
    session->Send("Hello, world!");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    server.Stop();
    server_thread.join();
    std::cout << "testClient end!!!" << std::endl;
}   


int testTCPServer() {
    // test_TcpServer();
    testClient();
    return 0;
}