#pragma once
/*
    此类包含Server，Clinet，NetLogic相关逻辑
    1. Server监听socket的连接，创建session与客户端通信
    2. Clinet连接Server，创建session与Server通信
    3. NetLogic处理消息的逻辑, 每次使用时需要继承NetLogic类，来传递给Server

*/



#include "NetLogic.hpp"
#include "TCPServer.hpp"
#include "TCPClinet.hpp"

#include "TCPHeader.hpp"
#include "TCPSession.hpp"
#include "AsioIOContextPool.hpp"

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



// ========================= TCPServer =========================
TCPServer::TCPServer(unsigned short port, std::shared_ptr<NetLogic> net_logic)
    : _acceptor(_io_context, tcp::endpoint(tcp::v4(), port))
    , _port(port) {
    _net_logic = net_logic;
    _net_logic->SetServer(this);
    std::cout << "Server start success, listen on port : " << _port << std::endl;
    startAccept();
}

void TCPServer::ClearSession(std::string session_id){
    if(_sessions.find(session_id) == _sessions.end()) {
        return;
    }
    _net_logic->OnClientDisconnected(session_id);
    std::lock_guard<std::mutex> lock(_mutex);
    _sessions.erase(session_id);
}

void TCPServer::startAccept() {
    auto &io_context = AsioIOContextPool::GetInstance()->GetIOContext();
    auto new_session = TCPSession::Create(io_context);
    new_session->SetRemoveCallback(std::bind(&TCPServer::ClearSession, this, std::placeholders::_1));
    new_session->SetRecvMsgCallback([this](const auto& msg) {
        _net_logic->Push(msg);
    });
    _net_logic->OnClientConnected(new_session->GetSessionId());
    _acceptor.async_accept(new_session->GetSocket(), std::bind(&TCPServer::handleAccept, this, new_session, std::placeholders::_1));
}

void TCPServer::handleAccept(std::shared_ptr<TCPSession> new_session, const error_code& error){
    if (!error) {
        new_session->Start();
        _sessions.insert(std::make_pair(new_session->GetSessionId(), new_session));
    } else {
        std::cout << "session accept failed, error is " << error.message() << std::endl;
    }

    startAccept();
}
// ======================== TCPServer =========================



// ======================== NetLogic =========================

inline void NetLogic::Send(const netMsgNode& msg) {
    TCPSession::Shared session = _server->GetSession(msg.SessionId);
    if(session) {
        session->Send(*msg.Data);
    }
}

inline void NetLogic::Send(const std::string& session_id, const std::vector<uint8_t>& data) {
    TCPSession::Shared session = _server->GetSession(session_id);
    if(session) {
        session->Send(data);
    }
}

//  ======================== NetLogic =========================



//   ======================== TestNetLogic =========================
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
// ======================== TestNetLogic =========================

