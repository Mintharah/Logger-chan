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
  LogManagerBuilder builder;
  builder.addConsoleSink();
  builder.addFileSink("/home/yasmine/Desktop/cpu_logs.txt");

  auto logManager = builder.build();

  for (int i = 0; i < 5; ++i) {
    LogMessage msg;
    msg.app_name = "TestApp";
    msg.context  = "CPU";
    msg.severity = "INFO";
    msg.time     = "NOW";
    msg.text     = "Hello " + std::to_string(i);

    logManager->log(std::move(msg));
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));
}

