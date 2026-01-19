#include "FileSinkImpl.hpp"
#include <fcntl.h>     // for open(), O_RDONLY, O_WRONLY, O_CREAT, etc.
#include <sys/types.h> // for ssize_t
#include <unistd.h>    // for read(), write(), close()

void FileSinkImpl::filePath(const std::string filepath) {
  FileSinkImpl::path = filepath;
}

void FileSinkImpl::writeMessage(const LogMessage &msg) {
  fd = open(path.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0644);
  if (fd < 0) {
    perror("Couldn't open file:(");
    return;
  }
  std::string line = "[" + msg.app_name + "][" + msg.context + "]" + "[" +
                     msg.severity + "][" + msg.time + "]" + "[" + msg.text +
                     "]\n";

  ssize_t n = write(fd, line.c_str(), line.size());
  if (n < 0) {
    perror("Couldn't write in the file:(");
    return;
  }
}