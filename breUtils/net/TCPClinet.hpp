#pragma once

/*
类似工厂模式，用于创建TCP客户端
*/

#ifdef ASIO_STANDALONE
#include <asio.hpp>
#include <system_error>
using std::system_error;
using namespace asio;
using error_code = asio::error_code;
#else
#include <boost/asio.hpp>
namespace asio = boost::asio;
using error_code = boost::system::error_code;
#endif

#include "TCPHeader.hpp"
#include "TCPSession.hpp"
#include "AsioIOContextPool.hpp"
#include <iostream>

using asio::ip::tcp;

class TCPClinet {
public:
    static TCPSession::Shared GetSession(std::string ip, int port, std::function<void(netMsgNode)> recvMsgCallback) {
        auto &io_context = AsioIOContextPool::GetInstance()->GetIOContext();
        auto new_session = TCPSession::Create(io_context);
        new_session->SetRemoveCallback([this](std::string session_id) {
            
        });
        new_session->SetRecvMsgCallback(recvMsgCallback);

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(ip, std::to_string(port));
        asio::connect(new_session->GetSocket(), endpoints);
        new_session->Start();

        return new_session;
    }

    ~TCPClinet() {
        std::cout << "Client stop" << std::endl;
    }
private:

private:


};



