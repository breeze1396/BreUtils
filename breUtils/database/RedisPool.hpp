#pragma once

#include "hiredis.h"

#include "../Singleton.hpp"
#include "../Timer.hpp"

#include <queue>
#include <atomic>
#include <mutex>

namespace bre {

class RedisPool {
public:
	RedisPool(size_t poolSize, const char* host, int port, const char* pwd)
		: _poolSize(poolSize)
        , _host(host)
        , _port(port)
        , _b_stop(false)
        , _pwd(pwd)
        , _check_timer(30'000) {
		for (size_t i = 0; i < _poolSize; ++i) {
			auto* context = redisConnect(host, port);
			if (context == nullptr || context->err != 0) {
				if (context != nullptr) {
					redisFree(context);
				}
				continue;
			}

			auto reply = (redisReply*)redisCommand(context, "AUTH %s", pwd);
			if (reply->type == REDIS_REPLY_ERROR) {
				std::cout << "认证失败" << std::endl;
				//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
				freeReplyObject(reply);
				continue;
			}

			//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
			freeReplyObject(reply);
			std::cout << "认证成功" << std::endl;
			_connections.push(context);
		}

        _check_timer.AsyncWait([this] {
            checkThread();
        }, true);
	}

	~RedisPool() {
        ClearConnections();
        _check_timer.Cancel();
	}

	void ClearConnections() {
		std::lock_guard<std::mutex> lock(_mutex);
		while (!_connections.empty()) {
			auto* context = _connections.front();
			redisFree(context);
			_connections.pop();
		}
	}

	redisContext* getConnection() {
		std::unique_lock<std::mutex> lock(_mutex);
		_cond.wait(lock, [this] { 
			if (_b_stop) {
				return true;
			}
			return !_connections.empty(); 
			});
		//如果停止则直接返回空指针
		if (_b_stop) {
			return  nullptr;
		}
		auto* context = _connections.front();
		_connections.pop();
		return context;
	}

	void returnConnection(redisContext* context) {
		std::lock_guard<std::mutex> lock(_mutex);
		if (_b_stop) {
			return;
		}
		_connections.push(context);
		_cond.notify_one();
	}

	void Close() {
		_b_stop = true;
		_cond.notify_all();
		check_thread_.join();
	}

private:
	void checkThread() {
		std::lock_guard<std::mutex> lock(_mutex);
		if (_b_stop) {
			return;
		}
		auto pool_size = _connections.size();
		for (int i = 0; i < pool_size && !_b_stop; i++) {
			auto* context = _connections.front();
			_connections.pop();
			try {
				auto reply = (redisReply*)redisCommand(context, "PING");
				if (!reply) {
					std::cout << "reply is null, redis ping failed: " << std::endl;
					_connections.push(context);
					continue;
				}
				freeReplyObject(reply);
				_connections.push(context);
			} catch(std::exception& e){
				std::cout << "Error keeping connection alive: " << e.what() << std::endl;
				redisFree(context);
				context = redisConnect(_host, _port);
				if (context == nullptr || context->err != 0) {
					if (context != nullptr) {
						redisFree(context);
					}
					continue;
				}

				auto reply = (redisReply*)redisCommand(context, "AUTH %s", _pwd);
				if (reply->type == REDIS_REPLY_ERROR) {
					std::cout << "认证失败" << std::endl;
					//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
					freeReplyObject(reply);
					continue;
				}

				//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
				freeReplyObject(reply);
				std::cout << "认证成功" << std::endl;
				_connections.push(context);
			}
		}
	}

private:
	std::atomic<bool> _b_stop;
	size_t _poolSize;

	std::string _host;
	std::string _pwd;
	int _port;

	std::queue<redisContext*> _connections;
	std::mutex _mutex;
	std::condition_variable _cond;

	bre::Timer _check_timer;
};

} // namespace bre
