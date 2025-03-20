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

#include "AsioIOContextPool.hpp"
#include "TCPSession.hpp"

#include <iostream>
#include <memory>
#include <functional>
#include <map>
#include <mutex>

using asio::ip::tcp;

/*
监听socket的连接，创建session与客户端通信
*/
class TCPServer {
public:
    TCPServer(unsigned short port)
        : _acceptor(_io_context, tcp::endpoint(tcp::v4(), port))
        , _port(port) {
        cout << "Server start success, listen on port : " << _port << endl;
	    startAccept();
    }

    //阻塞运行
    void Run() {
        _io_context.run();
    }

private:
    void startAccept() {
        auto &io_context = AsioIOContextPool::GetInstance()->GetIOContext();
        shared_ptr<TCPSession> new_session = make_shared<TCPSession>(io_context);
        new_session->SetRemoveCallback(std::bind(&TCPServer::ClearSession, this, placeholders::_1));
        _acceptor.async_accept(new_session->GetSocket(), std::bind(&TCPServer::handleAccept, this, new_session, placeholders::_1));
    }

    void handleAccept(shared_ptr<TCPSession> new_session, const error_code& error){
        if (!error) {
            new_session->Start();
            lock_guard<mutex> lock(_mutex);
            _sessions.insert(make_pair(new_session->GetSessionId(), new_session));
        } else {
            cout << "session accept failed, error is " << error.what() << endl;
        }
    
        startAccept();
    }

    void ClearSession(std::string session_id) {
        lock_guard<mutex> lock(_mutex);
        _sessions.erase(session_id);
    }

private:
    asio::io_context _io_context;
    tcp::acceptor _acceptor;
    unsigned short _port;

    std::map<std::string, shared_ptr<TCPSession>> _sessions;
	std::mutex _mutex;
};