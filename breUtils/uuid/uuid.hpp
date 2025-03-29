#pragma once

#include <chrono>
#include <mutex>
#include <thread>
#include <stdexcept>
#include <format>
#include <iostream>
#include <atomic>

namespace bre {

    struct IdGenOptions {
        uint64_t BaseTime{ 631123200000 }; // 基础时间（ms单位）
        uint8_t SeqBitLength{ 8 };         // 序列号位长
        uint8_t MaxSeqNumber{ 20 };        // 最大序列号
        uint8_t MinSeqNumber{ 5 };        // 最小序列号
    };

    // ID 生成器核心逻辑
    class SnowWorker {
    public:
        explicit SnowWorker(const IdGenOptions& options) {
            setOptions(options);
        }

        // 生成下一个 ID
        int64_t Next() {
            std::lock_guard lock(_mutex);
            auto currentTimeTick = getCurrentTimeTick();

            if (currentTimeTick > _lastTimeTick) {
                _lastTimeTick = currentTimeTick;
                _currentSeqNumber = _minSeqNumber;
            }
            else if (_currentSeqNumber > _maxSeqNumber) {
                _lastTimeTick++;
                _currentSeqNumber = _minSeqNumber;
            }

            return calculateId();
        }

    private:
        int64_t getCurrentTimeTick() const {
            using namespace std::chrono;
            auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            return now - static_cast<int64_t>(_baseTime);
        }

        // 计算 ID
        int64_t calculateId() {
            uint64_t id = (static_cast<uint64_t>(_lastTimeTick) << _seqBitLength) | _currentSeqNumber;
            _currentSeqNumber++;
            return static_cast<int64_t>(id);
        }

        // 设置配置选项
        void setOptions(const IdGenOptions& options) {
            if (options.BaseTime < 631123200000 || options.BaseTime > getCurrentTimeTick() + _baseTime) {
                throw std::invalid_argument("BaseTime error.");
            }
            _baseTime = options.BaseTime;

            if (options.SeqBitLength < 2 || options.SeqBitLength > 21) {
                throw std::invalid_argument("SeqBitLength error. (range:[2, 21])");
            }
            _seqBitLength = options.SeqBitLength;

            uint32_t maxSeqNumber = (1 << options.SeqBitLength) - 1;
            if (options.MaxSeqNumber > maxSeqNumber) {
				std::string msg = std::format("SeqBitLength: {}, 但是你的maxSeqNumber: {} 超过了", 
                                            options.SeqBitLength, maxSeqNumber);
                throw std::invalid_argument(msg);
            }

			if (options.MaxSeqNumber == 0) {
				_maxSeqNumber = maxSeqNumber;
			}
			else {
				_maxSeqNumber = options.MaxSeqNumber;
			}

            if (options.MinSeqNumber < 5 || options.MinSeqNumber > _maxSeqNumber) {
				std::string msg = std::format("MinSeqNumber error. (range:[5, {}])", _maxSeqNumber);
                throw std::invalid_argument("MinSeqNumber error.");
            }
            _minSeqNumber = options.MinSeqNumber;

            _lastTimeTick = 0;
            _currentSeqNumber = _minSeqNumber;
        }

    private:
        uint64_t _baseTime{};
        uint8_t _seqBitLength{};
        uint32_t _maxSeqNumber{};
        uint32_t _minSeqNumber{};
        int64_t _lastTimeTick{};
        uint32_t _currentSeqNumber{};
        std::mutex _mutex;
    };

    class IdGenerator {
    public:
        IdGenerator(const IdGenerator&) = delete;
        IdGenerator& operator=(const IdGenerator&) = delete;

        static IdGenerator& instance() {
            static IdGenerator instance;

            std::once_flag flag;
            std::call_once(flag, [&] {
                IdGenOptions options;
                instance._worker = std::make_unique<SnowWorker>(options);
            });
            return instance;
        }

        static int64_t Next() {
            return instance()._worker->Next();
        }

    private:
        IdGenerator() = default;
        std::unique_ptr<SnowWorker> _worker;
    };

} // namespace bre
