#ifndef LOGMESSAGE
#define LOGMESSAGE

#include <ostream>
#include <string>

class LogMessage {
public:
  std::string app_name;
  std::string context;
  std::string time;
  std::string severity;
  std::string text;

  LogMessage() = default;
  LogMessage(const LogMessage &) = default;
  LogMessage(LogMessage &&) = default;
  LogMessage &operator=(const LogMessage &) = default;
  LogMessage &operator=(LogMessage &&) = default;
  ~LogMessage() = default;
};

std::ostream &operator<<(std::ostream &os, const LogMessage &msg);

#endif