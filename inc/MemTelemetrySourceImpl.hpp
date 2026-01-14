#ifndef MEMTELEMETRYSOURCEIMP_HPP
#define MEMTELEMETRYSOURCEIMP_HPP

#include "ITelemetrySource.hpp"
#include "SafeFile.hpp"

class MemTelemetrySourceImpl : public ITelemetrySource {
    private:
    int fd = -1;
    SafeFile file;

    public:
    bool openSource() override;
    bool readSource(std::string& out) override;

};
#endif