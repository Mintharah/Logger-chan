#ifndef ILOGSINK
#define ILOGSINK

#include "LogMessage.hpp"

class ILogSink {
public:
  virtual ~ILogSink() = default;
  virtual void writeMessage(const LogMessage &msg) = 0;
};

#endif