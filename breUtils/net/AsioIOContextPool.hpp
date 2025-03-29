#pragma once

#include "../Singleton.hpp"

#ifdef ASIO_STANDALONE
#include <asio.hpp>
using namespace asio;
#else
#include <boost/asio.hpp>
namespace asio = boost::asio;
#endif

#include <vector>
#include <thread>
#include <iostream>

class AsioIOContextPool: public Singleton<AsioIOContextPool> {
	friend Singleton<AsioIOContextPool>;
public:
	using IOContext = asio::io_context;
	using Work = asio::executor_work_guard<asio::io_context::executor_type>;
	using WorkPtr = std::unique_ptr<Work>;
	~AsioIOContextPool();
	AsioIOContextPool(const AsioIOContextPool&) = delete;
	AsioIOContextPool& operator=(const AsioIOContextPool&) = delete;
	// 使用 round-robin 的方式返回一个 io_context --> 负载均衡
	asio::io_context& GetIOContext();
	void Stop();
private:
	AsioIOContextPool(std::size_t size = std::thread::hardware_concurrency());
	std::vector<IOContext>      _ioContext;
	std::vector<WorkPtr>        _works; // 保持 io_context 的工作
	std::vector<std::thread>    _threads;
	std::size_t                 _nextIOContext; // 下一个 io_context 的索引
};


AsioIOContextPool::AsioIOContextPool(std::size_t size):
        _ioContext(size), _works(size), _nextIOContext(0) {
	for (std::size_t i = 0; i < size; ++i) {
		_works[i] = std::make_unique<Work>(asio::make_work_guard(_ioContext[i]));
	}

    for (std::size_t i = 0; i < _ioContext.size(); ++i) {
        try {
            _threads.emplace_back([this, i]() {
                _ioContext[i].run();
            });
        } catch (const std::exception& e) {
            std::cerr << "Error starting thread: " << e.what() << '\n';
        }
    }
}

AsioIOContextPool::~AsioIOContextPool() {
    Stop();
    std::cout << "AsioIOContextPool destruct\n";
}

asio::io_context& AsioIOContextPool::GetIOContext() {
	auto& context = _ioContext[_nextIOContext++];
	if (_nextIOContext == _ioContext.size()) {
		_nextIOContext = 0;
	}
	return context;
}

void AsioIOContextPool::Stop(){
	//当iocontext已经绑定了读或写的监听事件后，还需要手动stop该服务。
	for (auto& context : _ioContext) {
		//把服务先停止
		context.stop();
	}

	for (auto& t : _threads) {
		t.join();
	}

	for (auto& work : _works) {
		work.reset();
	}
}
