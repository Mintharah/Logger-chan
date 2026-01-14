#include "MemTelemetrySourceImpl.hpp"
#include <fcntl.h>
#include <sstream> // parsing
#include <unistd.h>

bool MemTelemetrySourceImpl::openSource(){
    fd = open("/proc/meminfo", O_RDONLY);
    if(fd < 0){
        return false;
    }
    file = SafeFile(fd);
    return true;
}

bool MemTelemetrySourceImpl::readSource(std::string& out){
    if(!file.isOpen()){
        return false;
    }

    lseek(file.get(), 0, SEEK_SET);

    char buffer[256];
    ssize_t n = read(file.get(), buffer, sizeof(buffer));
    if(n <= 0){
        return false;
    }

    std::istringstream iss(buffer);
    std::string key;
    std::string unit;
    long value;

    long memTotal, memAvail;

    while(iss >> key >> value >> unit){
        if(key == "MemTotal:"){
            memTotal = value;
        }
        else if(key == "MemAvailable"){
            memAvail = value;
        }
    }

    double memUsage = ((static_cast<double>(memTotal - memAvail)/memTotal)*100);
    out = "Memory Usage: " + std::to_string(memUsage) + "%\n";

    return true;
}