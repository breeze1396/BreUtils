#pragma once

#include "Broadcast.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>

void test_broadcast() {
    asio::io_context io_context;

    std::string broadcast_address = "192.168.1.255";
    unsigned short port = 30001;

    bre::BroadcastSender sender(io_context, broadcast_address, port);
    bre::BroadcastReceiver receiver(io_context, port);

    std::thread io_thread([&io_context]() {
        io_context.run();
    });

    std::string test_message;
    {  
        std::string in_message = "Hello, Broadcast!";
        test_message = in_message;
        sender.Send(in_message);
        in_message = "";
    }

    std::string received_message = receiver.Receive(1);
    std::cout << "Received message: " << received_message << std::endl;

    assert(received_message == test_message);


    io_context.stop();
    io_thread.join();

    std::cout << "Broadcast test passed!" << std::endl;
}

int testBroadcast() {
    test_broadcast();
    return 0;
}