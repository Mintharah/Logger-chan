#include "LogSinkFactory.hpp"

std::unique_ptr<ILogSink> LogSinkFactory::create(LogSinkType_enum type) {
  switch (type) {
  case (LogSinkType_enum::CONSOLE):
    return std::make_unique<ConsoleSinkImpl>();
  case (LogSinkType_enum::FILE): {
    auto sink = std::make_unique<FileSinkImpl>();
    sink->filePath("/home/yasmine/Desktop/cpu_logs.txt");
    return sink;
  }
  case (LogSinkType_enum::SOCKET):
    return nullptr; // havent implemented it yet :3
  default:
    return nullptr;
  }
}