/*
未完成
组播相互发现工具
*/

#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <atomic>

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

using namespace boost;
using asio::ip::udp;
using tcp = asio::ip::tcp;

constexpr std::string multicast_address = "225.6.7.4";
constexpr unsigned short multicast_port = 8674;

struct HostInfo {
    std::string state; // 0: 加入 1: 退出
    std::string Ip;
    std::string Name;
    std::string Mac;
    HostInfo() = default;
    HostInfo(std::string s, std::string ip, std::string name, std::string mac): state(s), Ip(ip), Name(name), Mac(mac) {}

    std::string ToString() {
      // 0,192.1.1.1,test,00:00:00:00:00:00
        return state + ","+ Ip + "," + Name + "," + Mac;
    }

    bool Parse(const std::string& str) {
        int pos = 0;
        int new_pos = 0;
        try {
            std::vector<std::string> vec = StringSplit(str, ",");
            if(vec.size() != 4) {
                throw std::runtime_error("Error parsing message: " + str);
            }
            state = vec[0];
            Ip = vec[1];
            Name = vec[2];
            Mac = vec[3];
            if(state != "0" && state != "1") {
                throw std::runtime_error("Error parsing state not 0 1: " + str);
            }
            if(Ip.empty()) {
                throw std::runtime_error("Error parsing Ip.empty: " + str);
            }
        } catch(const std::exception& e) {
            std::cerr << e.what() << '\n'; 
            return false;
        }
      
        return true;
    }

    std::vector<std::string> StringSplit(const std::string& str, const std::string& spliter) {
        std::vector<std::string> vec;
        // 去除前后空格和/n
        std::string::size_type pos = 0;
        std::string::size_type new_pos = 0;

        while((new_pos = str.find(spliter, pos)) != std::string::npos) {
            vec.push_back(str.substr(pos, new_pos - pos));
            pos = new_pos + spliter.size();
        }
        vec.push_back(str.substr(pos));
        return vec;
    }
};


class MulticastSender {
public:
    MulticastSender(asio::io_context& io_context, const std::string& multicast_address)
        : _socket(io_context) {

        _sendto_endpoint = udp::endpoint(asio::ip::make_address_v4(multicast_address), multicast_port);
        _socket.open(_sendto_endpoint.protocol());
        // _socket.set_option(asio::socket_base::broadcast(true));
        _socket.set_option(asio::socket_base::reuse_address(true));
        _socket.set_option(asio::ip::multicast::join_group(asio::ip::make_address_v4(multicast_address)));
        _socket.set_option(asio::ip::multicast::enable_loopback(true));
        _socket.set_option(asio::ip::multicast::hops(3));
    }

    void Send(const std::string& message) {
        std::shared_ptr<std::string> message_ptr = std::make_shared<std::string>(message);
        _socket.async_send_to(asio::buffer(*message_ptr), _sendto_endpoint,
        [&](std::error_code ec, std::size_t /*bytes_sent*/) {
            if (ec) {
                std::cout << "Message sent err: " << ec.message() << std::endl;
            }
        });
    }

private:
    udp::socket _socket;
    udp::endpoint _sendto_endpoint;
};


class MulticastReceiver {
public:
    MulticastReceiver(asio::io_context& io_context, const std::string& multicast_address, unsigned short port)
        : _socket(io_context) {
        _listen_endpoint = udp::endpoint(asio::ip::make_address_v4(multicast_address), port);
        _socket.open(_listen_endpoint.protocol());
        _socket.set_option(asio::socket_base::reuse_address(true));
        _socket.bind(_listen_endpoint);

        _socket.set_option(asio::ip::multicast::join_group(asio::ip::make_address_v4(multicast_address)));
    }

    void Receive() {
        auto recv_buffer = std::make_shared<std::array<char, 1024>>();
        _socket.async_receive_from(asio::buffer(*recv_buffer), _listen_endpoint,
        [this, recv_buffer](std::error_code ec, std::size_t bytes_recvd) {
            if (ec) {
                std::cout << "Receive err: " << ec.message() << std::endl;
                return;
            }
            auto msg = std::string(recv_buffer->data(), bytes_recvd);
            // std::cout << "Received: size:" << bytes_recvd << " " << msg << std::endl;

            {
                std::lock_guard<std::mutex> lock(_message_mutex);
                _message_queue.push({recv_buffer->begin(), recv_buffer->begin() + bytes_recvd});
                _message_cv.notify_one();
            }

            Receive();
        });
    }

    std::string GetStr(int timeout = -1) {
        std::unique_lock<std::mutex> lock(_message_mutex);
        if(timeout > 0) {
            _message_cv.wait_for(lock, std::chrono::seconds(timeout), [this] { return !_message_queue.empty() || _is_stop; });
        } else {
            _message_cv.wait(lock, [this] { return !_message_queue.empty() || _is_stop; });
        }

        if (_message_queue.empty()) {
            return "";
        }

        if(_is_stop) {
            return "";
        }

        auto message = _message_queue.front();
        _message_queue.pop();

        auto str = std::string(message.begin(), message.end());
        return str;
    }

    void Close() {
        _is_stop = true;
        // 关闭异步接受

    }

private:
    bool _is_stop = false;
    std::queue<std::vector<uint8_t>> _message_queue;
    std::mutex _message_mutex;
    std::condition_variable _message_cv;

    udp::socket _socket;
    udp::endpoint _listen_endpoint;
};


class Finder {
public:
    Finder(): _multicast_endpoint(asio::ip::make_address_v4(_multicast_address), multicast_port),
        _sender(_io_context, _multicast_address),
        _receiver(_io_context, _multicast_address, multicast_port) {
        _is_stop = false;

        initSelf();
        _local_ips.push_back("192.168.0.1");
        _local_ips.push_back("192.168.1.1");
        _receiver.Receive();
 

        _send_thread = std::thread(&Finder::send_thread, this);
        _receive_thread = std::thread(&Finder::receive_thread, this);
    }

    void Run() {
        _io_context.run();
    }

    void Close() {
        _is_stop = true;
        _io_context.stop();
        if(_send_thread.joinable()) {
            _send_thread.join();
        }
        if(_receive_thread.joinable()) {
            _receive_thread.join();
        }
    }

    std::vector<HostInfo> GetHosts() {
        std::vector<HostInfo> hosts;
        for(auto& [ip, host] : _hosts) {
            hosts.push_back(host);
        }
        return hosts;
    }



    ~Finder() {
        if(!_is_stop) {
            Close();
        }
    }


private:
    void initSelf(){
        _self.state = "0";
        _self.Ip = "127.0.0.1";
        _self.Name = "test";
        _self.Mac = "00:00:00:00:00:00";
    }

    std::vector<std::string> get_local_ips() {
        std::vector<std::string> local_ips;
        try {
            tcp::resolver resolver(_io_context);
            std::string hostname = asio::ip::host_name();
            tcp::resolver::results_type results = resolver.resolve(hostname, "");

            for (const auto& result : results) {
                local_ips.push_back(result.endpoint().address().to_string());
            }
        } catch (std::exception& e) {
            std::cerr << "Error getting local IPs: " << e.what() << "\n";
        }
        return local_ips;
    }


    void receive_thread() {
        std::array<char, 1024> recv_buffer;
        while (true) {
            if(_is_stop) {
                break;
            }
            
            std::string message = _receiver.GetStr(1);
            if (message.empty()) {
                std::this_thread::sleep_for(std::chrono::seconds(3));
                std::cout << "Receive timeout" << std::endl;
                continue;
            }

            parse_message(message);
        }
    }

    bool parse_message(const std::string& message) {
        // 解析消息
        HostInfo host;
        if (!host.Parse(message)) {
            std::cerr << "Error parsing message: " << message << std::endl;
            return false;
        }

        if (host.Ip == _self.Ip) {
            return false;
        }


        if (host.state == "1") {   // 退出
            std::cout << "out>>>>:" <<host.Ip << " " << host.Name << " " << host.Mac << std::endl;
            _hosts.erase(host.Ip);
        } else {    // 加入
            std::cout << "in>>>>:" <<host.Ip << " " << host.Name << " " << host.Mac << std::endl;
            if (!this->_hosts.contains(host.Ip)){
                std::cout << "join "<<host.Ip << "\n";
                this->_hosts[host.Ip] = host;
            }
        }
        return true;
    }


    void send_thread() {
        // 对于本机所有的ip进行发送
        while (true) {
            if(_is_stop) {
                break;
            }
            for(auto& local_ip : _local_ips) {
                _self.Ip = local_ip;
                std::string message = _self.ToString();
                _sender.Send(message);
            }
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }

    }

    bool _is_stop = false;

    HostInfo _self;
    std::vector<std::string> _local_ips; // 本机的所有ip
    std::unordered_map<std::string, HostInfo> _hosts; // 所有的主机信息 key: ip

    std::string _multicast_address = multicast_address;

    // 网络相关
    asio::io_context _io_context;
    udp::endpoint _multicast_endpoint;  // 组播地址

    MulticastSender _sender;
    MulticastReceiver _receiver;

    std::thread _send_thread;
    std::thread _receive_thread;
};

void test_mutilcast_sender_receiver(){
     asio::io_context io_context;
    MulticastSender sender(io_context, multicast_address);

    MulticastReceiver receiver(io_context, multicast_address, multicast_port);
    receiver.Receive();


    std::thread t([&]{
        for(int i = 0; i < 5; ++i) {
            sender.Send("123456abc");
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    io_context.run();
    t.join();
}


inline int test() {
    Finder finder;

    std::thread ([&]{
        finder.Run();
    }).detach();

    for(int i = 0; i < 100; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        auto hosts = finder.GetHosts();

        if(hosts.empty()) {
            std::cout << "No hosts" << std::endl;
            continue;   
        }

        for(auto& host : hosts) {
            std::cout << "host Info: " << host.Ip << " " << host.Name << " " << host.Mac << std::endl;
        }
        std::cout << "======================" << std::endl;


    }

    finder.Close();


    return 0;
}
