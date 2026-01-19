#ifndef LOGMANAGER
#define LOGMANAGER

#include "ILogSink.hpp"
#include "LogMessage.hpp"
#include "RingBuffer.hpp"
#include <memory>
#include <vector>

class LogManager {
private:
  RingBuffer<LogMessage>
      buffer; /* where the messages will be stored temporarily */
  std::vector<std::unique_ptr<ILogSink>>
      sinks; /* where the sinks will be stored */
public:
  LogManager(size_t bufferSize = 1024) : buffer(bufferSize){} 

  void addSink(std::unique_ptr<ILogSink> sink); /* stores available sinks */
  void log(LogMessage&& msg); /* stores the messages into a buffer */
  void flush(void);         /* sends the messasges to all sinks */
};

#endif