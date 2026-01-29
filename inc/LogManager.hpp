#ifndef LOGMANAGER_HPP
#define LOGMANAGER_HPP

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
private:
  RingBuffer<LogMessage>
      buffer; /* where the messages will be stored temporarily */
  std::vector<std::unique_ptr<ILogSink>>
      sinks; /* where the sinks will be stored */

  std::thread worker;
  std::atomic<bool> running;
  std::condition_variable cv;
  std::mutex mtx;

public:
  LogManager(size_t bufferSize = 1024) : buffer(bufferSize), running(true) {
    worker = std::thread(&LogManager::workerLoop, this);
  }

  ~LogManager() {
    LogManager::stop();
    if (worker.joinable()) {
      worker.join();
    }
  }

  void addSink(std::unique_ptr<ILogSink> sink); /* stores available sinks */
  void log(LogMessage &&msg); /* stores the messages into a buffer */
  void workerLoop(void);         /* sends the messasges to all sinks */
  void stop(void);
};

#endif