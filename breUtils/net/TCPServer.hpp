#pragma once

#ifdef ASIO_STANDALONE
#include <asio.hpp>
using namespace asio;
using error_code = asio::error_code;
#else
#include <boost/asio.hpp>
namespace asio = boost::asio;
using boost::system::error_code;
using error_code = boost::system::error_code;
#endif

#include "AsioIOContextPool.hpp"
#include "TCPSession.hpp"

#include <iostream>
#include <memory>
#include <functional>
#include <map>
#include <mutex>

using asio::ip::tcp;

class NetLogic;

/*
监听socket的连接，创建session与客户端通信
*/
class TCPServer {
public:
    TCPServer(unsigned short port, std::shared_ptr<NetLogic> net_logic);

    //阻塞运行
    void Run() {
        _io_context.run();
    }

    void Stop() {
        std::lock_guard<std::mutex> lock(_mutex);
        for(auto session : _sessions) {
            session.second->Stop();
        }
        _io_context.stop();
    }

    TCPSession::Shared GetSession(std::string session_id) {
        std::lock_guard<std::mutex> lock(_mutex);
        if(_sessions.find(session_id) == _sessions.end()) {
            return nullptr;
        }
        return _sessions[session_id];
    }

    ~TCPServer() {
        Stop();
        std::lock_guard<std::mutex> lock(_mutex);
        _sessions.clear();
        std::cout << "Server stop" << std::endl;
    }

private:
    void startAccept();

    void handleAccept(std::shared_ptr<TCPSession> new_session, const error_code& error);

    void ClearSession(std::string session_id);

private:
    asio::io_context _io_context;
    tcp::acceptor _acceptor;
    unsigned short _port;

    std::map<std::string, std::shared_ptr<TCPSession>> _sessions;
	std::mutex _mutex;

    std::shared_ptr<NetLogic> _net_logic;
};

