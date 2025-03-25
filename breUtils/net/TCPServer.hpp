#pragma once

#ifdef ASIO_STANDALONE
#include <asio.hpp>
using namespace asio;
using error_code = asio::error_code;
#else
#include <boost/asio.hpp>
namespace asio = boost::asio;
using boost::system::error_code;
#endif

#include "AsioIOContextPool.hpp"
#include "TCPSession.hpp"

#include <iostream>
#include <memory>
#include <functional>
#include <map>
#include <mutex>

using asio::ip::tcp;


class TCPServer;

class NetLogic {
public:
    using Shared = std::shared_ptr<NetLogic>;

    NetLogic(){
        _thread = std::thread([this](){
            while(!_is_closed) {
                process();
            }
        });
    }

    virtual ~NetLogic() {
        _is_closed = true;
        if(_thread.joinable()) {
            _thread.join();
        }
    }

    void Push(const netMsgNode& msg) {
        _queue.Push(msg);
    }

    void SetServer(TCPServer* server) {
        _server = server;
    }

private:
    void process() {
        netMsgNode msg;
        while (_queue.Pop(msg, 10)) {
            handleMessage(msg);
        }
    }
protected:
    void Send(const netMsgNode& msg);
protected:
    virtual void handleMessage(const netMsgNode& msg) = 0;

    bre::BlockQueue<netMsgNode> _queue;

    TCPServer* _server = nullptr;

    std::atomic<bool> _is_closed = false;
    std::thread _thread;
};


/*
监听socket的连接，创建session与客户端通信
*/
class TCPServer {
public:
    TCPServer(unsigned short port, NetLogic::Shared net_logic)
        : _acceptor(_io_context, tcp::endpoint(tcp::v4(), port))
        , _port(port) {
        _net_logic = net_logic;
        _net_logic->SetServer(this);
        std::cout << "Server start success, listen on port : " << _port << std::endl;
	    startAccept();
    }

    //阻塞运行
    void Run() {
        _io_context.run();
    }

    void Stop() {
        for(auto& session : _sessions) {
            session.second->Stop();
        }
        _sessions.clear();
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
        std::cout << "Server stop" << std::endl;
    }

private:
    void startAccept() {
        auto &io_context = AsioIOContextPool::GetInstance()->GetIOContext();
        auto new_session = TCPSession::Create(io_context);
        new_session->SetRemoveCallback(std::bind(&TCPServer::ClearSession, this, std::placeholders::_1));
        new_session->SetRecvMsgCallback([this](const auto& msg) {
            _net_logic->Push(msg);
        });
        _acceptor.async_accept(new_session->GetSocket(), std::bind(&TCPServer::handleAccept, this, new_session, std::placeholders::_1));
    }

    void handleAccept(std::shared_ptr<TCPSession> new_session, const error_code& error){
        if (!error) {
            new_session->Start();
            _sessions.insert(std::make_pair(new_session->GetSessionId(), new_session));
        } else {
            std::cout << "session accept failed, error is " << error.what() << std::endl;
        }
    
        startAccept();
    }

    void ClearSession(std::string session_id) {
        if(_sessions.find(session_id) == _sessions.end()) {
            return;
        }
        _sessions.erase(session_id);
    }

private:
    asio::io_context _io_context;
    tcp::acceptor _acceptor;
    unsigned short _port;

    std::map<std::string, std::shared_ptr<TCPSession>> _sessions;
	std::mutex _mutex;

    NetLogic::Shared _net_logic;
};


inline void NetLogic::Send(const netMsgNode& msg) {
    TCPSession::Shared session = _server->GetSession(msg.SessionId);
    if(session) {
        session->Send(*msg.Data);
    }
}



class TestNetLogic : public NetLogic {
    protected:
        void handleMessage(const netMsgNode& msg) override;
};

inline void TestNetLogic::handleMessage(const netMsgNode& msg) {
    std::cout << "TestNetLogic: " << msg.SessionId << " " 
    << std::string(msg.Data->begin(), msg.Data->end()) << std::endl;
    TCPSession::Shared session = _server->GetSession(msg.SessionId);
    if(session) {
        session->Send(*msg.Data);
    }
}