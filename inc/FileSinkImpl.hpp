#ifndef FILE_SINK_IMPL
#define FILE_SINK_IMPL

#include "ILogSink.hpp"

class FileSinkImpl : public ILogSink {
private:
  int fd;
  std::string path;
public:
  void filePath(const std::string filepath);
  void writeMessage(const LogMessage &msg) override;
};

#endif