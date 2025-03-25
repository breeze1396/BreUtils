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

#include "../BlockQueue.hpp"
#include "netMsgNode.hpp"
#include "TCPHeader.hpp"
#include "netMsgNode.hpp"
#include <memory>


class TCPSession : public std::enable_shared_from_this<TCPSession> {
public:
    using Shared = std::shared_ptr<TCPSession>;

    static Shared Create(asio::io_context& io_context) {
        return Shared(new TCPSession(io_context));
    }
    Shared Shared_from_this() {
        return shared_from_this();
    }

    TCPSession(asio::io_context& io_context): _socket(io_context) {
        _session_id = generateSessionId();
        _is_closed = false;
    }

    ~TCPSession() {
        if(_write_thread.joinable()) {
            _write_thread.join();
        }

        _socket.close();
        
        std::cout << "Session destructed: " << _session_id << std::endl;
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
    void SetRecvMsgCallback(std::function<void(netMsgNode)> cb) {
        _cb_recv_msg = cb;
    }

    void Start() {
        std::cout << "Session start: " << _session_id << std::endl;
        // 开始监听
        _write_thread = std::thread([this]{
            writeThread();
        });
        asyncReadHeader();
    }

    void Stop() {
        auto self = shared_from_this();
        _is_closed = true;
        if(_cb_remove_from_server){ _cb_remove_from_server(_session_id);}
    }

    void Send(std::vector<uint8_t> data) {
        _queue_send.Push(std::move(data));
    }
    void Send(const std::string& data) {
        _queue_send.Push(std::vector<uint8_t>(data.begin(), data.end()));
    }
    void Send(void* data, size_t size) {
        _queue_send.Push(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(data), reinterpret_cast<uint8_t*>(data)+size));
    }

private:
    static std::string generateSessionId() {
        return std::to_string(_session_id_counter++);
    }

    void asyncReadHeader() {
        asio::async_read(_socket, asio::buffer(_header_data, TCPHeader::HeaderSize), 
                        [this](std::error_code ec, std::size_t length){
            if(ec) {
                std::cerr << "Read header failed: " << ec.message() << std::endl;
                Stop();
                return;
            }
            if(!_header.Parse(std::vector<uint8_t>(_header_data, _header_data+TCPHeader::HeaderSize))) {
                std::cerr << "Parse header failed" << std::endl;
                return;
            }
            asyncReadData(_header.FileSize);
        });
    }

    void asyncReadData(int length) {
        auto recv_data = std::make_shared<std::vector<uint8_t>>(length);
        asio::async_read(_socket, asio::buffer(*recv_data), asio::transfer_exactly(length),
                        [this, recv_data](std::error_code ec, std::size_t length){
            if(ec) {
                std::cerr << "Read data failed: " << ec.message() << std::endl;
                Stop();
                return;
            }
            netMsgNode msg;
            msg.SessionId = _session_id;
            msg.Data = recv_data;
            if(_cb_recv_msg) {
                _cb_recv_msg(msg);
            }
            // std::cout << "Recv data: " << std::string(recv_data->begin(), recv_data->end()) << std::endl;
            asyncReadHeader();
        });
    }

    void writeThread(){
        while(!_is_closed) {
            std::vector<uint8_t> data;
            try {
                if(_queue_send.Pop(data, 10)) {
                    // 写头 写数据
                    asio::write(_socket, asio::buffer(_header.GetHeaderData(data.size())));
                    asio::write(_socket, asio::buffer(data));
                }
            } catch(const std::exception& e) {
                std::cerr << "writeThread error: " <<e.what() << '\n';
            }
        }
    }

private:
    std::atomic<bool> _is_closed = false;

    tcp::socket _socket;
    std::string _session_id;

    inline static std::atomic<uint64_t> _session_id_counter = 0;

    std::function<void(std::string)> _cb_remove_from_server;
    std::function<void(netMsgNode)> _cb_recv_msg;

    TCPHeader _header;
    char _header_data[TCPHeader::HeaderSize];

    bre::BlockQueue<std::vector<uint8_t>> _queue_send;

    std::thread _write_thread;
};