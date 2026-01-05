#include "LogMessage.hpp"

std::ostream& operator<<(std::ostream& os, const LogMessage& msg) {
  os << '[' << msg.app_name << ']'
     << '[' << msg.context << ']'
     << '[' << msg.severity << ']'
     << '[' << msg.time << ']'
     << '[' << msg.text << ']';

  return os;
}