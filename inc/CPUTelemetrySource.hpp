#ifndef CPUTELEMTRYSOURCE_HPP
#define CPUTELEMTRYSOURCE_HPP

#include "ITelemetrySource.hpp"
#include "SafeFile.hpp"

class CPUTelemtrySourceImpl : public ITelemetrySource {
private:
  int fd = -1;
  SafeFile file;

public:
  CPUTelemtrySourceImpl() = default;
  ~CPUTelemtrySourceImpl() = default;

  bool openSource() override;
  bool readSource(std::string &out) override;
};

#endif