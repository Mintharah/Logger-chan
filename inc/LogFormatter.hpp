#ifndef LOGFORMATTER_HPP
#define LOGFORMATTER_HPP

#include "LogMessage.hpp"
#include "Policies.hpp"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>

template <typename Policy> class LogFormatter {
public:
  // Generate the timestamp
  static std::string currentTimeStamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;

    localtime_r(&t, &tm);

    char buff[32];
    std::strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", &tm);

    return std::string(buff);
  }

  // Generate payload/description
  static std::string msgDescription(float val) {
    return std::to_string(val) + " " + std::string(Policy::unit);
  }

  static std::optional<LogMessage> formatDataToLogMsg(std::string &raw) {
    raw.erase(std::remove(raw.begin(), raw.end(), '\n'), raw.end());
    float val = std::stof(raw);

    LogMessage msg;
    msg.context = toStringSrc(Policy::context);
    msg.severity = toStringSeverity(Policy::inferSeverity(val));
    msg.time = currentTimeStamp();
    msg.text = msgDescription(val);
    msg.app_name = "Telemetry App";

    return msg;
  }
};

#endif