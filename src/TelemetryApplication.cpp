#include "TelemetryApplication.hpp"
#include "CPUTelemetrySource.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"
#include "LogFormatter.hpp"
#include "Policies.hpp"
#include "SomeIPTelemetrySourceImpl.hpp"

#include <chrono>
#include <fstream>

TelemetryApplication::TelemetryApplication(const std::string &configPath) {
  loadConfig(configPath);
  setUpLogManager();
  setUpTelemetrySources();
}

TelemetryApplication::~TelemetryApplication() {
  running = false;
  for (auto &t : telemetryThreads) {
    if (t.joinable()) {
      t.join();
    }
  }
}

void TelemetryApplication::loadConfig(const std::string &path) {
  std::ifstream file(path);
  file >> config;
}

void TelemetryApplication::setUpLogManager() {
  size_t bufferSize = config["logging"]["buffer_size"];
  logManager = std::make_unique<LogManager>(bufferSize);

  if (config["sinks"]["console"]) {
    logManager->addSink(std::make_unique<ConsoleSinkImpl>());
  }

  if (config["sinks"]["file"]["enabled"]) {
    auto sink = std::make_unique<FileSinkImpl>();
    sink->filePath(config["sinks"]["file"]["path"]);
    logManager->addSink(std::move(sink));
  }
}

void TelemetryApplication::setUpTelemetrySources() {
  if (config["telemetry_sources"]["cpu"]["enabled"]) {
    auto rateMs = config["telemetry_sources"]["cpu"]["rate_ms"];
    auto cpuSrc = std::make_unique<CPUTelemtrySourceImpl>();
    cpuSrc->openSource();

    telemetryThreads.emplace_back(
        [this, cpu = std::move(cpuSrc), rateMs]() mutable {
          telemetryLoop(std::move(cpu), rateMs, [](std::string &raw) {
            return LogFormatter<CpuPolicy>::formatDataToLogMsg(raw);
          });
        });
  }
  if (config["telemetry_sources"]["someip"]["enabled"]) {
    auto rateMs = config["telemetry_sources"]["someip"]["rate_ms"];
    auto someipSrc = std::make_unique<SomeIPTelemetrySourceImpl>();
    if (someipSrc->openSource()) {
      telemetryThreads.emplace_back(
          [this, someip = std::move(someipSrc), rateMs]() mutable {
            telemetryLoop(std::move(someip), rateMs, [](std::string &raw) {
              return LogFormatter<SomeIPPolicy>::formatDataToLogMsg(raw);
            });
          });
    } else {
      std::cerr << "Failed to open SomeIP telemetry source" << std::endl;
    }
  }
}

void TelemetryApplication::telemetryLoop(
    std::unique_ptr<ITelemetrySource> source, int rateMs,
    std::function<std::optional<LogMessage>(std::string &)> formatter) {
  while (running) {
    std::string raw;
    if (source->readSource(raw)) {
      auto msg = formatter(raw);
      if (msg) {
        logManager->log(std::move(*msg));
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(rateMs));
  }
}

void TelemetryApplication::start() {
  while (running) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}