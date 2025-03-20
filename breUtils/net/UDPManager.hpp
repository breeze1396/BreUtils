#pragma once

// 如果不包含boost/asio.hpp, 则使用asio.hpp
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

#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <cstdint>
#include <iostream>
#include <thread>
#include <chrono>   
#include <system_error>

using namespace std::chrono_literals;

#pragma pack(push, 1)
struct PacketHeader {
    uint32_t header_signature = 0x12345678;
    uint32_t group_id;
    uint32_t packet_id = 0;
    uint32_t total_packets = 0;
    uint64_t total_size = 0;
};
#pragma pack(pop)

class UdpSender {
public:
    UdpSender(asio::io_context& io_context, const std::string& host, unsigned short port)
        : socket_(io_context), endpoint_(asio::ip::udp::resolver(io_context).resolve(host, std::to_string(port)).begin()->endpoint()) {
        socket_.open(endpoint_.protocol());
    }

    void Send(const uint8_t* data, size_t size) {
        const size_t max_udp_payload = 9216; // 基于实验得到的最大 UDP 数据包大小
        const size_t max_payload_size = max_udp_payload - sizeof(PacketHeader);
        uint32_t current_group_id = ++group_id_;
        size_t total_packets = (size + max_payload_size - 1) / max_payload_size;

        for (size_t offset = 0, packet_id = 0; offset < size; offset += max_payload_size, ++packet_id) {
            size_t current_size = std::min(size - offset, max_payload_size);
            PacketHeader header{
                0x12345678, 
                current_group_id, 
                static_cast<uint32_t>(packet_id),
                static_cast<uint32_t>(total_packets), 
                static_cast<uint64_t>(size)
            };

            std::vector<uint8_t> packet_data;
            packet_data.reserve(sizeof(PacketHeader) + current_size);
            packet_data.insert(packet_data.end(), reinterpret_cast<const uint8_t*>(&header), 
                              reinterpret_cast<const uint8_t*>(&header) + sizeof(PacketHeader));
            packet_data.insert(packet_data.end(), data + offset, data + offset + current_size);

            socket_.send_to(asio::buffer(packet_data), endpoint_);

            if(packet_id % 35 == 0){
                std::this_thread::sleep_for(900us); // 限制发送速度
            }
        }
        std::cout << "Send " << size << " bytes in " << total_packets << " packets" << std::endl;
    }

private:
    asio::ip::udp::socket socket_;
    asio::ip::udp::endpoint endpoint_;
    uint32_t group_id_ = 0;
};

class UdpReceiver {
public:
    using Callback = std::function<void(const uint8_t*, size_t)>;

    UdpReceiver(asio::io_context& io_context, unsigned short port)
        : socket_(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)) {
        buffer_.resize(9250); 
    }

    void Receive(Callback callback) {
        callback_ = std::move(callback);
        DoReceive();
    }

private:
    struct GroupInfo {
        uint32_t total_packets = 0;
        uint64_t total_size = 0;
        std::unordered_map<uint32_t, std::vector<uint8_t>> packets;
    };

    void DoReceive() {
        socket_.async_receive_from(
            asio::buffer(buffer_), sender_endpoint_,
            [this](const std::error_code& ec, std::size_t bytes_transferred) {
                if (!ec && bytes_transferred >= sizeof(PacketHeader)) {
                    HandlePacket(buffer_.data(), bytes_transferred);
                } else {
                    std::cout << "Receive error: " << ec.message() << std::endl;
                }

                DoReceive();
            });
    }

    void HandlePacket(const uint8_t* data, size_t size) {
        const PacketHeader* header = reinterpret_cast<const PacketHeader*>(data);
        if (header->header_signature != 0x12345678) return;

        auto& group_info = m_groups[header->group_id];
        if (group_info.total_packets == 0) {
            group_info.total_packets = header->total_packets;
            group_info.total_size = header->total_size;
        }

        size_t payload_size = size - sizeof(PacketHeader);
        if (payload_size > 0) {
            group_info.packets[header->packet_id].assign(data + sizeof(PacketHeader), 
                                                        data + sizeof(PacketHeader) + payload_size);
        }

        if (IsGroupComplete(header->group_id)>0) {
            auto merged = MergeGroup(header->group_id);
            if (!merged.empty() && callback_) {
                callback_(merged.data(), merged.size());
            }
            m_groups.erase(header->group_id);
        }
    }

    // -2: 错误，数据传输异常
    // -1: 严重错误，找不到对应的组
    //  0: 组未完成
    //  1: 数据组完成
    //  2: 超时，且90%的数据已经接收
    //  3: 超时，且70%的数据已经接收
    //  4: 超时，且50%的数据未接收 
    int IsGroupComplete(uint32_t group_id) {
        auto it = m_groups.find(group_id);
        if (it == m_groups.end()) return -1;

        const auto& info = it->second;
        if (info.packets.size() != info.total_packets) {
            return 0;
        }

        for (uint32_t i = 0; i < info.total_packets; ++i) {
            if (info.packets.find(i) == info.packets.end()) return -2;
        }

        return 1;
    }

    std::vector<uint8_t> MergeGroup(uint32_t group_id) {
        auto it = m_groups.find(group_id);
        if (it == m_groups.end()) return {};

        std::vector<uint8_t> merged;
        merged.reserve(it->second.total_size);
        for (uint32_t i = 0; i < it->second.total_packets; ++i) {
            const auto& packet = it->second.packets[i];
            merged.insert(merged.end(), packet.begin(), packet.end());
        }
        return merged;
    }

    asio::ip::udp::socket socket_;
    asio::ip::udp::endpoint sender_endpoint_;
    std::vector<uint8_t> buffer_;

    std::unordered_map<uint32_t, GroupInfo> m_groups;

    Callback callback_;
};
