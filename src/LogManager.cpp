#include "LogManager.hpp"

void LogManager::addSink(std::unique_ptr<ILogSink> sink) {
  std::lock_guard<std::mutex> lock(mtx);
  sinks.push_back(std::move(sink));
}

void LogManager::log(LogMessage &&msg) {
  if (buffer.tryPush(std::move(msg))) {
    cv.notify_one();
  }
}

void LogManager::workerLoop() {
  LogMessage msg;

  while (true) {
    { /* sleep unless the thread is stopped or the buffer is not empty (: */
      std::unique_lock<std::mutex> uL(mtx);
      cv.wait(uL, [&]() { return (!running || !buffer.isEmpty()); });
    }

    /* create a local sink buffer holding pointers to sink */
    std::vector<ILogSink*> localSinks; 
    {
      std::lock_guard<std::mutex> lock(mtx);
      for(auto &s : sinks){
        localSinks.push_back(s.get());
      }
    }

    /* drain all available messsages */
    while (buffer.tryPop(msg)) {
      for (auto &sink : localSinks) { /* used localSink buffer to not lock the mtx for long and avoid deadlocks (: */
        sink->writeMessage(msg);
      }
    }

    /* exit only when the thread is stopped */
    if (!running && buffer.isEmpty()) {
      break;
    }
  }
}

void LogManager::stop() {
  running = false;
  cv.notify_all();
}
