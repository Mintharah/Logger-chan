#include "CPUTelemetrySource.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileTelemetrySourceImpl.hpp"
#include "LogManager.hpp"
#include "MemTelemetrySourceImpl.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

int main() {
  LogManager logger;
  logger.addSink(std::make_unique<ConsoleSinkImpl>());

  FileTelemetrySourceImpl src(
      "/home/yasmine/Desktop/Linux_Project/FileLogging.txt");

  if (!src.openSource()) {
    std::cout << "Failed to open telemetry source\n";
    return 1;
  }

  CPUTelemtrySourceImpl cpu;
  if (!cpu.openSource()) {
    std::cout << "Failed to open CPU telemetry source\n";
  }

  MemTelemetrySourceImpl mem;
  if (!mem.openSource()) {
    std::cout << "Failed to open Mem telemetry source\n";
  }

  while (true) {
    std::string reading;

    cpu.readSource(reading);
    std::cout << reading << std::endl;
    mem.readSource(reading);
    std::cout << reading << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
}
