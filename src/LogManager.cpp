#include "LogManager.hpp"

void LogManager::addSink(std::unique_ptr<ILogSink> sink) {
  LogManager::sinks.push_back(std::move(sink));
}

void LogManager::log(LogMessage msg) {
  LogManager::buffer.push_back(std::move(msg));
}

void LogManager::flush(void) {
  for (const auto &message : buffer) {
    for (const auto &sink : sinks) {
      sink->writeMessage(message);
    }
  }
  buffer.clear();
}