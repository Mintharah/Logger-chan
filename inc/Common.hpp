#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>

enum class LogSinkType_enum { CONSOLE, FILE, SOCKET };
enum class SeverityLvl_enum { CRITICAL, WARNING, INFO };
enum class TelemetrySrc_enum { CPU, GPU, RAM };

std::string toStringSeverity(SeverityLvl_enum lvl);
std::string toStringSrc(TelemetrySrc_enum src);

#endif