#include "CPUTelemetrySource.hpp"
#include <fcntl.h>
#include <sstream> // parsing
#include <unistd.h>

bool CPUTelemtrySourceImpl::openSource() {
  fd = open("/proc/stat", O_RDONLY);
  if (fd < 0) {
    return false;
  }
  file = SafeFile(fd);
  return true;
}

bool CPUTelemtrySourceImpl::readSource(std::string &out) {
  if (!file.isOpen()) {
    return false;
  }
  
  lseek(file.get(), 0, SEEK_SET);

  char buffer[256] = {0};
  ssize_t n = read(file.get(), buffer, sizeof(buffer) - 1);
  if (n <= 0) {
    return false;
  }

  std::istringstream iss(buffer);
  std::string cpu;
  long user, nice, system, idle;

  iss >> cpu >> user >> nice >> system >> idle;
  if (cpu != "cpu") {
    return false;
  }

  long busy = user + nice + system; // cpu formula
  long total = busy + idle;

  double load = (static_cast<double>(busy) / total) * 100.0;

  out = std::to_string(load);
  return true;
}