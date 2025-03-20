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


using asio::ip::tcp;

#include <memory>


class TCPSession : public std::enable_shared_from_this<TCPSession> {
public:
    TCPSession(asio::io_context& io_context): _socket(io_context) {
        _session_id = generateSessionId();
    }

    tcp::socket& GetSocket() {
        return _socket;
    }

    std::string GetSessionId() {
        return _session_id;
    }

    void SetRemoveCallback(std::function<void(std::string)> cb) {
        _cb_remove_from_server = cb;
    }

    void Start() {
        
    }

    void Stop() {
        if(_cb_remove_from_server){ _cb_remove_from_server(_session_id);}
        _socket.close();
    }
private:
    static std::string generateSessionId() {
        return std::to_string(_session_id_counter++);
    }
private:
    tcp::socket _socket;
    std::string _session_id;

    static std::atomic<uint64_t> _session_id_counter;

    std::function<void(std::string)> _cb_remove_from_server;
};