#ifndef LOGSINKFACTORY_HPP
#define LOGSINKFACTORY_HPP
#include "Common.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"
#include "ILogSink.hpp"
#include "memory"

class LogSinkFactory {
public:
  static std::unique_ptr<ILogSink> create(LogSinkType_enum type);
};

#endif