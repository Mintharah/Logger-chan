#ifndef LOGMESSAGE
#define LOGMESSAGE

#include <string>
#include <ostream>

class LogMessage {
public:
  std::string app_name;
  std::string context;
  std::string time;
  std::string severity;
  std::string text;

};

std::ostream &operator<<(std::ostream &os, const LogMessage &msg);

#endif