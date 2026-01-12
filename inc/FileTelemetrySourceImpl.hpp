#ifndef FILETELEMETRYSOURCEIMPL_HPP
#define FILETELEMETRYSOURCEIMPL_HPP

#include "ITelemetrySource.hpp"
#include "SafeFile.hpp"
#include <string>

class FileTelemetrySourceImpl : public ITelemetrySource {
private:
  std::string path;
  SafeFile file;

public:
  explicit FileTelemetrySourceImpl(std::string path);
  ~FileTelemetrySourceImpl() = default;
  
  bool openSource() override;
  bool readSource(std::string &out) override;
};

#endif