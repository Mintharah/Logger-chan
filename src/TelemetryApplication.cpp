#include "TelemetryApplication.hpp"
#include "CPUTelemetrySource.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"
#include "LogFormatter.hpp"
#include "Policies.hpp"

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

void TelemetryApplication::loadConfig(const std::string& path){
    std::ifstream file(path);
    file >> config;
}

void TelemetryApplication::setUpLogManager(){
    size_t bufferSize = config["logging"]["buffer_size"];
    logManager = std::make_unique<LogManager>(bufferSize);

    if(config["sinks"]["console"]){
        logManager->addSink(std::make_unique<ConsoleSinkImpl>());
    }

    if(config["sinks"]["file"]["enabled"]){
        auto sink = std::make_unique<FileSinkImpl>();
        sink->filePath(config["sinks"]["file"]["path"]);
        logManager->addSink(std::move(sink));
    }
}

void TelemetryApplication::setUpTelemetrySources(){
    if(config["telemetry_sources"]["cpu"]["enabled"]){
        auto rateMs = config["telemetry_sources"]["cpu"]["rate_ms"];
        auto cpuSrc = std::make_unique<CPUTelemtrySourceImpl>();
        cpuSrc->openSource();
        
        telemetryThreads.emplace_back(&TelemetryApplication::telemetryLoop,
        this, std::move(cpuSrc), rateMs);
    }
}

void TelemetryApplication::telemetryLoop(std::unique_ptr<ITelemetrySource> source, int rateMs){
    while(running){
        std::string raw;
        if(source->readSource(raw)){
            auto msg = LogFormatter<CpuPolicy>::formatDataToLogMsg(raw);
            if(msg){
                logManager->log(std::move(*msg));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(rateMs));
    }
}

void TelemetryApplication::start(){
    while(running){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}