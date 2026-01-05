#ifndef CONSOLE_SINK_IMPL
#define CONSOLE_SINK_IMPL

#include "ILogSink.hpp"

class ConsoleSinkImpl : public ILogSink {
public:
  void writeMessage(const LogMessage &msg) override;
};

#endif