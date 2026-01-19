#include "CPUTelemetrySource.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"
#include "LogFormatter.hpp"
#include "LogManager.hpp"
#include "LogSinkFactory.hpp"
#include "LogManagerBuilder.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

int main() {
  // Log Sink Factory
  /*
  auto consoleSink = LogSinkFactory::create(LogSinkType_enum::CONSOLE);
  logManager.addSink(std::move(consoleSink));

  auto fileSink = LogSinkFactory::create(LogSinkType_enum::FILE);
  logManager.addSink(std::move(fileSink));
  */

  // Log Manager Builder
  LogManagerBuilder manager;
  manager.addConsoleSink();
  manager.addFileSink("/home/yasmine/Desktop/cpu_logs.txt");
  auto logManager = manager.build();

  // Open CPU source
  CPUTelemtrySourceImpl cpuSource;
  if (!cpuSource.openSource()) {
    std::cerr << "Failed to open CPU telemetry source\n";
    return 1;
  }

  // Loop to read telemetry
  for (int i = 0; i < 5; ++i) {
    std::string raw;
    if (cpuSource.readSource(raw)) {
      auto optMsg = LogFormatter<CpuPolicy>::formatDataToLogMsg(raw);
      if (optMsg) {
        logManager->log(std::move(*optMsg));
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  // Flush messages to sinks
  logManager->flush();

  return 0;
}
