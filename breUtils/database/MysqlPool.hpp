#pragma once

#include "../Defer.hpp"
#include "../Timer.hpp"

#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>

#include <thread>
#include <memory>
#include <queue>
#include <mutex>

namespace bre
{
	

class SqlConnection {
public:
	SqlConnection(sql::Connection* con, int64_t lasttime)
		: _con(con)
		, _last_oper_time(lasttime) {}
	std::unique_ptr<sql::Connection> _con;
	int64_t _last_oper_time;
};

class MySqlPool {
public:
	MySqlPool(const std::string& url, const std::string& user, const std::string& pass, const std::string& schema, int poolSize)
		: _url(url)
		, _user(user)
		, _pass(pass)
		, _schema(schema)
		, _poolSize(poolSize)
		, _b_stop(false)
		,_check_timer(60'000) {
		try {
			for (int i = 0; i < _poolSize; ++i) {
				sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
				auto*  con = driver->connect(_url, _user, _pass);
				con->setSchema(_schema);
				// 获取当前时间戳
				auto currentTime = std::chrono::system_clock::now().time_since_epoch();
				long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
				_pool.push(std::make_unique<SqlConnection>(con, timestamp));
			}

			_check_timer.AsyncWait([this] {
				checkConnection();
			}, true)		


		} catch (sql::SQLException& e) {
			std::cout << "mysql pool init failed during connection setup, error is: " << e.what() << std::endl;
		} catch (std::exception& e) {
			std::cout << "mysql pool init failed during general setup, error is: " << e.what() << std::endl;
		}
	}

	void checkConnection() {
		int poolSize = 0;
		{
			std::lock_guard<std::mutex> guard(_mutex);
			poolSize = _pool.size();
		}
		
		while (poolSize > 0) {
			poolSize--;

			if(_b_stop) {
				return;
			}

			std::unique_ptr<SqlConnection> con;
			{
				std::lock_guard<std::mutex> guard(_mutex);
				if (_pool.empty()) {
					break;
				}
				con = std::move(_pool.front());
				_pool.pop();
			}

			auto currentTime = std::chrono::system_clock::now().time_since_epoch();
			long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();

			if (timestamp - con->_last_oper_time >= 30) {
				try {
					std::unique_ptr<sql::Statement> stmt(con->_con->createStatement());
					stmt->executeQuery("SELECT 1");
					con->_last_oper_time = timestamp;
				} catch (sql::SQLException& e) {
					std::cout << "Error keeping connection alive during checkConnection, error is: " << e.what() << std::endl;
					// 重新创建连接并替换旧的连接
					sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
					auto* newcon = driver->connect(_url, _user, _pass);
					newcon->setSchema(_schema);
					con->_con.reset(newcon);
					con->_last_oper_time = timestamp;
				}
			}

			{
				std::lock_guard<std::mutex> guard(_mutex);
				_pool.push(std::move(con));
			}
		}
	}

	std::unique_ptr<SqlConnection> getConnection() {
		std::unique_lock<std::mutex> lock(_mutex);
		_cond.wait(lock, [this] { 
			if (_b_stop) {
				return true;
			}		
			return !_pool.empty(); });
		if (_b_stop) {
			return nullptr;
		}
		std::unique_ptr<SqlConnection> con(std::move(_pool.front()));
		_pool.pop();
		return con;
	}

	void returnConnection(std::unique_ptr<SqlConnection> con) {
		std::unique_lock<std::mutex> lock(_mutex);
		if (_b_stop) {
			return;
		}
		_pool.push(std::move(con));
		_cond.notify_one();
	}

	void Close() {
		_b_stop = true;
		_cond.notify_all();
		_check_timer.Cancel();
	}

	~MySqlPool() {
		Close();

		std::unique_lock<std::mutex> lock(_mutex);
		while (!_pool.empty()) {
			_pool.pop();
		}
	}

private:
	std::string _url;
	std::string _user;
	std::string _pass;
	std::string _schema;
	int _poolSize;
	std::queue<std::unique_ptr<SqlConnection>> _pool;
	std::mutex _mutex;
	std::condition_variable _cond;
	std::atomic<bool> _b_stop;

	Timer _check_timer;
};


} // namespace bre