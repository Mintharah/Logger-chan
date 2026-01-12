#include "LogManager.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileTelemetrySourceImpl.hpp"

#include <chrono>
#include <thread>
#include <memory>
#include <iostream>

int main() {
  LogManager logger;
  logger.addSink(std::make_unique<ConsoleSinkImpl>());

  FileTelemetrySourceImpl src("/home/yasmine/Desktop/Linux_Project/FileLogging.txt");

  if (!src.openSource()) {
    std::cout << "Failed to open telemetry source\n";
    return 1;
  }

  while (true) {
    std::string reading;

    if (src.readSource(reading)) {
      LogMessage msg;
      msg.app_name = "TelemetryApp";
      msg.context  = "ShellScript";
      msg.severity = "INFO";
      msg.time     = "now";
      msg.text     = reading;

      logger.log(msg);
      logger.flush();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
}
