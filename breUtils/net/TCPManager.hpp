#pragma once
/*
一对一的TCP通信，不能用于多对一的情况，发送端比较完备
*/


#include "TCPHeader.hpp"

#include <queue>
#include <memory>
#include <array>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>

#ifdef ASIO_STANDALONE
#include <asio.hpp>
using namespace asio;
using error_code = asio::error_code;
#else
#include <boost/asio.hpp>
namespace asio = boost::asio;
using error_code = boost::system::error_code;
#endif


using namespace asio::ip;
using namespace std::chrono_literals;

class TCPSender {
public:
    using Shared = std::shared_ptr<TCPSender>;
    static Shared Create(std::string host, int port) {
        return Shared(new TCPSender(host, port));
    }


    TCPSender(std::string host, int port)
        : _socket(m_io_context)
        , m_host(host)
        , m_port(port)
    {   
        try{
            Connect();

            _send_thread = std::thread([this]{
                try {
                    sendThread();
                } catch(const std::exception& e) {
                    std::cout << "Maybe the server or receiver is not started" << std::endl;
                    std::cerr << "sendThread error: " << e.what() << '\n';
                }
            
            });
        } catch (const std::exception& e) {
            std::cerr << "TCPSender Exception: " << e.what() << std::endl;
            std::cout << "maybe the server or receiver is not started" << std::endl;
        }
        _is_closed = false;
    }

    void Connect(){
        int retry = 3;
        bool connected = false;
        while (retry > 0) {
            try {
                tcp::resolver resolver(m_io_context);
                auto endpoints = resolver.resolve(m_host, std::to_string(m_port));
                asio::connect(_socket, endpoints);
                // // 设置长连接
                // asio::socket_base::keep_alive keep_alive(true);
                // _socket.set_option(keep_alive);
                // // 设置发送超时
                // asio::socket_base::send_buffer_size send_buffer_size(1024*1024*10);
                // _socket.set_option(send_buffer_size);
                connected = true;
                break;
            } catch (const std::exception& e) {
                retry--;
                if(retry == 0) {
                    throw e;
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        if(connected) {
            std::cout << "Connected to " << m_host << ":" << m_port << std::endl;
        } else {
            std::cerr << "Failed to connect to " << m_host << ":" << m_port << std::endl;
            throw std::runtime_error("Failed to connect to " + m_host + ":" + std::to_string(m_port));
        }
    }

    void Run(){
        m_io_context.run();
    }

    void Send(const std::string& data) {
        std::unique_lock<std::mutex> lock(_mutex);
        _queue.emplace(std::vector<uint8_t>(data.begin(), data.end()));
        _cv.notify_one();
    }

    void Send(const std::vector<uint8_t> data) {
        std::unique_lock<std::mutex> lock(_mutex);
        _queue.emplace(std::move(data));
        _cv.notify_one();
    }

    void Send(void* data, size_t size) {
        std::unique_lock<std::mutex> lock(_mutex);
        _queue.emplace(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(data), reinterpret_cast<uint8_t*>(data)+size));
        _cv.notify_one();
    }

    // 直接发送数据
    void SendDirect(void* data, size_t size) {
        // 写头
        asio::write(_socket, asio::buffer(_header.GetHeaderData(size)));
        // 写文件
        asio::write(_socket, asio::buffer((uint8_t*)data, size));
    }

    void Close() {
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _is_closed = true;
            _cv.notify_all();
        }

        if (_send_thread.joinable()) {
            _send_thread.join();
        }

        if(_socket.is_open()) {
            _socket.close();
        }
    }

    ~TCPSender() {
        Close();
        std::cout << "TCPSender destructed" << std::endl;
    }

private:
    void sendThread() {
        while (true) {
            std::vector<uint8_t> data;
            { 
                std::unique_lock<std::mutex> lock(_mutex);
                _cv.wait(lock, [this]{
                    return !_queue.empty() || _is_closed;
                });

                if (_is_closed && _queue.empty()) {
                    break;
                }

                data = _queue.front();
                _queue.pop();
            }
            // 写头
            asio::write(_socket, asio::buffer(_header.GetHeaderData(data.size())));
            // 写文件
            asio::write(_socket, asio::buffer(data));
        }
    }    
private:
    bool _is_closed = true;

    TCPHeader _header;

    std::string m_host;
    int m_port;
    asio::io_context m_io_context;
    tcp::socket _socket;


    std::queue<std::vector<uint8_t>> _queue;
    std::mutex _mutex;
    std::condition_variable _cv;

    std::thread _send_thread;
};


class TCPReceiver {
public:
<<<<<<< HEAD
    using Shared = std::shared_ptr<TCPReceiver>;
    static Shared Create(int port) {
        return std::make_shared<TCPReceiver>(port);
    }

    TCPReceiver(int port):m_port(port), _socket(m_io_context)
    {
=======
    TCPReceiver(int port):m_port(port), _socket(m_io_context) {
>>>>>>> 055da1d71142e4fd2f7528a9c96e96c75aea3643
        std::cout << "TCPReceiver listen: " << port <<'\n';
        startConnect();
        _recv_thread = std::thread([this]{
            recvThread();
        });
    }

    ~TCPReceiver() {
        Close();
        std::cout << "Reader destructed" << std::endl;
    }

    void Close() {
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _is_closed = true;
            _cv.notify_all();
        }
        
        if (_recv_thread.joinable()) {
            _recv_thread.join();
        }
    }

    std::vector<uint8_t> Get() {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv.wait(lock, [this]{
            return !_queue.empty();
        });

        if (_queue.empty()) {
            return {};
        }

        auto data = std::move(_queue.front());
        _queue.pop();
        return data;
    }

    std::string GetStr() {
        auto data = Get();
        std::cout << "GetStr: " << std::string(data.begin(), data.end()) << std::endl;
        return std::string(data.begin(), data.end());
    }

private:
    void startConnect(){
        tcp::acceptor acceptor(m_io_context, tcp::endpoint(tcp::v4(), m_port));
        _socket = acceptor.accept();
<<<<<<< HEAD
        // acceptor.async_accept(_socket, [this](const error_code& ec) {
        //     if (!ec) {
        //         std::cout << "Connection accepted" << std::endl;
        //         _recv_thread = std::thread([this]{
        //             recvThread();
        //         });
        //     } else {
        //         std::cerr << "Accept error: " << ec.message() << std::endl;
        //     }
        // });
=======
>>>>>>> 055da1d71142e4fd2f7528a9c96e96c75aea3643
    }

    std::vector<uint8_t> read_data_from_socket() {
        asio::streambuf request_buffer;
        error_code error;
        // 读32字节的头部
        int readsize = asio::read(_socket, request_buffer, asio::transfer_exactly(32), error);
        if (error) {
            if(error == asio::error::eof) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                connect();
                return {};
                // std::cout << "Connection closed" << std::endl;
            } else throw std::system_error(error); // Handle different error.
        }
        if(readsize == 0) {
            return {};
        }
        
        _header.Parse(std::vector<uint8_t>(asio::buffers_begin(request_buffer.data()), asio::buffers_end(request_buffer.data())));
        if (_header.Data[0] != 'B' || _header.Data[1] != 'R' || _header.Data[2] != 'E' || _header.Data[3] != 'Z') {
            std::cerr << "Invalid header" << std::endl;
            return {};
        }
        size_t length = _header.FileSize;
        // 消耗32字节头
        request_buffer.consume(32);

        // 读数据
        std::vector<uint8_t> data(length);
        int offset = 0;
        while (offset < length)
        {
            readsize = asio::read(_socket, request_buffer, asio::transfer_exactly(length - offset), error);
            if (error) {
                if(error == asio::error::eof) {
                    std::cout << "Connection closed" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    startConnect();
                    return {};
                } else throw std::system_error(error);
            }
            std::copy(asio::buffers_begin(request_buffer.data()), 
                      asio::buffers_begin(request_buffer.data()) + readsize, 
                      data.begin() + offset);
            
            offset += readsize;
            request_buffer.consume(readsize);
            // std::cout << "Read " << readsize << " bytes" << std::endl;
        }
    
        return data;
    }

    void recvThread() {
        while (true) {
            try {
                if(_is_closed) {
                    break;
                }
                auto receivedData = read_data_from_socket();
                if(receivedData.empty()) [[unlikely]] {
                    // std::cout << "Received empty data" << std::endl;
                    continue;
                }
                _queue.push(std::move(receivedData));
                _cv.notify_one();
            } catch (const std::exception& e) {
                // std::cerr << "Exception: " << e.what() << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
private:
    bool _is_closed = false; 
    TCPHeader _header;
    
    asio::io_context m_io_context;
    tcp::socket _socket;
    int m_port = 0;

    std::queue<std::vector<uint8_t>> _queue;
    std::mutex _mutex;
    std::condition_variable _cv;

    std::thread _recv_thread;   
};

<<<<<<< HEAD


// class TCPReceiver {
// public:
//     using Shared = std::shared_ptr<TCPReceiver>;
//     static Shared Create(int port) {
//         return std::make_shared<TCPReceiver>(port);
//     }

//     TCPReceiver(int port):m_port(port), _acceptor(m_io_context, tcp::endpoint(tcp::v4(), m_port))
//     {
//         std::cout << "TCPReceiver listen: " << port <<'\n';
//         startAccept();
//         _recv_thread = std::thread([this]{
//             m_io_context.run();
//         });
//         std::cout << "TCPReceiver thread started" << std::endl;
//     }

//     ~TCPReceiver() {
//         Close();
//         std::cout << "Reader destructed" << std::endl;
//     }

//     void Close() {
//         {
//             std::unique_lock<std::mutex> lock(_mutex);
//             _is_closed = true;
//             _cv.notify_all();
//         }
        
//         if (_recv_thread.joinable()) {
//             _recv_thread.join();
//         }
//     }

//     std::vector<uint8_t> Get() {
//         std::unique_lock<std::mutex> lock(_mutex);
//         _cv.wait(lock, [this]{ return !_queue.empty(); });

//         if (_queue.empty()) { return {}; }

//         auto data = std::move(_queue.front());
//         _queue.pop();
//         return data;
//     }

//     std::string GetStr() {
//         auto data = Get();
//         return std::string(data.begin(), data.end());
//     }

// private:
//     void startAccept() {
//         auto socket = std::make_shared<tcp::socket>(m_io_context);
//         _acceptor.async_accept(*socket, [this, socket](const error_code& ec) {
//             if (!ec) {
//                 std::cout << "Connection accepted" << std::endl;
//                 startReceive(socket);
//             } else {
//                 std::cerr << "Accept error: " << ec.message() << std::endl;
//             }
//             if (!_is_closed) {
//                 startAccept();
//             }
//         });
//     }

//     void startReceive(std::shared_ptr<tcp::socket> socket) {
//         auto buffer = std::make_shared<asio::streambuf>();
//         asio::async_read(*socket, *buffer, asio::transfer_exactly(32), [this, socket, buffer](const error_code& ec, std::size_t) {
//             if (!ec) {
//                 auto data = std::vector<uint8_t>(asio::buffers_begin(buffer->data()), asio::buffers_end(buffer->data()));
//                 _header.Parse(data);
//                 size_t length = _header.FileSize;
//                 buffer->consume(32);

//                 auto dataBuffer = std::make_shared<std::vector<uint8_t>>(length);
//                 asio::async_read(*socket, asio::buffer(*dataBuffer), asio::transfer_exactly(length), [this, socket, dataBuffer](const error_code& ec, std::size_t) {
//                     if (!ec) {
//                         std::unique_lock<std::mutex> lock(_mutex);
//                         _queue.push(std::move(*dataBuffer));
//                         _cv.notify_one();
//                     } else {
//                         std::cerr << "Receive error: " << ec.message() << std::endl;
//                     }
//                 });
//             } else {
//                 std::cerr << "Header receive error: " << ec.message() << std::endl;
//             }
//         });
//     }

// private:
//     int m_port = 0;
    
//     bool _is_closed = false; 
//     TCPHeader _header;
    
//     asio::io_context m_io_context;
//     tcp::acceptor _acceptor;


//     std::queue<std::vector<uint8_t>> _queue;
//     std::mutex _mutex;
//     std::condition_variable _cv;

//     std::thread _recv_thread;   
// };
=======
>>>>>>> 055da1d71142e4fd2f7528a9c96e96c75aea3643
