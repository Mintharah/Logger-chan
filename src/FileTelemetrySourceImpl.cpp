#include "FileTelemetrySourceImpl.hpp"
#include <fcntl.h>
#include <unistd.h>

FileTelemetrySourceImpl::FileTelemetrySourceImpl(std::string path)
    : path(std::move(path)) {}

bool FileTelemetrySourceImpl::openSource() {
  int fd = open(path.c_str(), O_RDONLY);
  if (fd < 0) {
    return false;
  }
  file = SafeFile(fd);
  return true;
}

bool FileTelemetrySourceImpl::readSource(std::string &out) {
  if (!file.isOpen()) {
    return false;
  }

  char buffer[1024];
  ssize_t n = read(file.get(), buffer, sizeof(buffer) - 1);
  if (n <= 0) {
    return false;
  }

  buffer[n] = '\0';
  out = buffer;

  // trim trailing newline(s) and carriage returns
  while (!out.empty() && (out.back() == '\n' || out.back() == '\r'))
    out.pop_back();

  return true;
}