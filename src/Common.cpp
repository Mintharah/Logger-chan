#include "Common.hpp"

std::string toStringSeverity(SeverityLvl_enum lvl) {
  switch (lvl) {
  case (SeverityLvl_enum::CRITICAL):
    return "CRITICAL";
  case (SeverityLvl_enum::WARNING):
    return "WARNING";
  case (SeverityLvl_enum::INFO):
    return "INFO";
  default:
    return "UNKNOWN";
  }
}

std::string toStringSrc(TelemetrySrc_enum src) {
  switch (src) {
  case (TelemetrySrc_enum::CPU):
    return "CPU";
  case (TelemetrySrc_enum::GPU):
    return "GPU";
  case (TelemetrySrc_enum::RAM):
    return "RAM";
  default:
    return "UNKNOWN";
  }
}