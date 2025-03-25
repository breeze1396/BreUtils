#pragma once
#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#ifdef ASIO_STANDALONE
#include <asio.hpp>
#include <system_error>
using std::system_error;
using namespace asio;
#else
#include <boost/asio.hpp>
namespace asio = boost::asio;
using boost::system::error_code;
#endif

#include "TCPManager.hpp"
#include "TCPServer.hpp"
#include "AsioIOContextPool.hpp"
#include "TCPSession.hpp"

#include <thread>
#include <chrono>

static void sendThread(int id, int port) {
    TCPSender sender("127.0.0.1", port);
    for(int i = 0; i < 10; i++) {
        int sleep_time = rand() % 100 + 100;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        std::string msg = "id: " + std::to_string(id) + "sleep time: " + std::to_string(sleep_time);
        sender.Send(msg);
    }
    sender.Send("Hello, world!");
    sender.Close();
}

class MyNetLogic : public NetLogic {
public:
    void handleMessage(const netMsgNode& msg) override {
        std::cout << "Recv msg: " << std::string(msg.Data->begin(), msg.Data->end()) << std::endl;
        Send(msg);
    }

};

static void test_TcpServer() {
    std::cout << "TCPServer starts and listens on the specified port" << std::endl;
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

int testTCPServer() {
    test_TcpServer();
    return 0;
}