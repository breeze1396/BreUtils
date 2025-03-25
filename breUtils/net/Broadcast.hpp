#pragma once

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

#include "../BlockQueue.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>


/*
发送数据尽可能小，不应该超过1400字节
*/

namespace bre {

class BroadcastSender {
public:
    BroadcastSender(asio::io_context& io_context, const std::string& broadcast_address, unsigned short port)
        : _endpoint(asio::ip::make_address_v4(broadcast_address), port),
            _socket(io_context, _endpoint.protocol()) {
        // 启用广播选项
        _socket.set_option(asio::socket_base::broadcast(true));
        _socket.set_option(asio::ip::multicast::enable_loopback(true));
    }

    void Send(const std::string& message) {
        auto in_msg = std::make_shared<std::string>(message);
        _socket.async_send_to(
            asio::buffer(*in_msg), _endpoint,
            [this, in_msg](std::error_code ec, std::size_t bytes_sent) {
                if (!ec) {
                    // pass
                } else {
                    std::cerr << "Send failed: " << ec.message() << std::endl;
                }
        });
    }

private:
    asio::ip::udp::endpoint _endpoint;
    asio::ip::udp::socket _socket;
};

class BroadcastReceiver {
public:
    BroadcastReceiver(asio::io_context& io_context, unsigned short port)
        : _socket(io_context) {
        // 绑定到指定端口，监听所有接口
        asio::ip::udp::endpoint listen_endpoint(asio::ip::udp::v4(), port);
        _socket.open(listen_endpoint.protocol());
        _socket.set_option(asio::socket_base::reuse_address(true));
        _socket.bind(listen_endpoint);

        // 启动接收循环
        do_receive();
    }

    std::string Receive(int timeout_ms = -1) {
        std::string msg;
        _queue.Pop(msg, timeout_ms);
        return msg;
    }

private:
    void do_receive() {
        _socket.async_receive_from(
            asio::buffer(_data, max_length), _sender_endpoint,
            [this](std::error_code ec, std::size_t bytes_recvd) {
                if (!ec && bytes_recvd > 0) {
                    _queue.Push(std::string(_data, bytes_recvd));
                } else {
                    // 处理错误（如缓冲区溢出）
                    std::cerr << "Receive error: " << ec.message() << std::endl;
                }
                do_receive();
            });
    }

    asio::ip::udp::socket _socket;
    asio::ip::udp::endpoint _sender_endpoint;
    BlockQueue<std::string> _queue;
    
    static constexpr size_t max_length = 1024;
    char _data[max_length];
};

} // namespace bre