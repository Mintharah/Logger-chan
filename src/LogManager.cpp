#include "LogManager.hpp"

void LogManager::addSink(std::unique_ptr<ILogSink> sink) {
  LogManager::sinks.push_back(std::move(sink));
}

void LogManager::log(LogMessage &&msg) {
  if (!buffer.tryPush(std::move(msg))) {
    LogManager::flush();
  }
}

void LogManager::flush(void) {
  LogMessage msg;
  while (buffer.tryPop(msg)) {
    for (auto &sink : sinks) {
      sink->writeMessage(msg);
    }
  }
}