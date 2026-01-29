#include "LogManager.hpp"

void LogManager::addSink(std::unique_ptr<ILogSink> sink) {
  LogManager::sinks.push_back(std::move(sink));
}

void LogManager::log(LogMessage &&msg) {
  if (buffer.tryPush(std::move(msg))) {
    cv.notify_one();
  }
}

void LogManager::workerLoop() {
  LogMessage msg;

  while (running || !buffer.isEmpty()) {
    if (buffer.tryPop(msg)) {
      for (auto &sink : sinks) {
        sink->writeMessage(msg);
      }
    } else {
      std::unique_lock<std::mutex> lock(mtx);
      cv.wait(lock);
    }
  }
}

void LogManager::stop() {
  running = false;
  cv.notify_all();
}
