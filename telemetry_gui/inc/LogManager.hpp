#pragma once
#include "ILogSink.hpp"
#include "LogMessage.hpp"
#include "RingBuffer.hpp"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class LogManager {
public:
    explicit LogManager(size_t bufferSize = 1024)
        : buffer(bufferSize), running(true), m_capacity(bufferSize) {
        worker = std::thread(&LogManager::workerLoop, this);
    }
    ~LogManager() {
        stop();
        if (worker.joinable()) worker.join();
    }

    void addSink(std::unique_ptr<ILogSink> sink);
    void log(LogMessage &&msg);
    void workerLoop();
    void stop();

    // Safe to read from any thread (GUI poll timer)
    size_t bufferSize()     const { return buffer.size();  }
    size_t bufferCapacity() const { return m_capacity;     }

private:
    RingBuffer<LogMessage>                 buffer;
    std::vector<std::unique_ptr<ILogSink>> sinks;
    std::thread                            worker;
    std::atomic<bool>                      running;
    std::condition_variable                cv;
    std::mutex                             mtx;
    const size_t                           m_capacity;
};
