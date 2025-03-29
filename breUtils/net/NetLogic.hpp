#pragma once

#include "../BlockQueue.hpp"
#include "TCPHeader.hpp"
#include "netMsgNode.hpp"

#include <thread>
#include <atomic>
#include <iostream>
#include <memory>

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

    // 处理连接
    virtual void OnClientConnected(const std::string& session_id){
        std::cout << "OnClientConnected: " << session_id << std::endl;
    }
    // 处理断开连接
    virtual void OnClientDisconnected(const std::string& session_id){
        std::cout << "OnClientDisconnected: " << session_id << std::endl;
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
    void Send(const std::string& session_id, const std::vector<uint8_t>& data);
protected:
    virtual void handleMessage(const netMsgNode& msg) = 0;

    bre::BlockQueue<netMsgNode> _queue;

    TCPServer* _server = nullptr;

    std::atomic<bool> _is_closed = false;
    std::thread _thread;
};
