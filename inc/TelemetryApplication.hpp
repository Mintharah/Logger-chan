#ifndef TELEMETRYAPPLICATION_HPP
#define TELEMETRYAPPLICATION_HPP

#include "ITelemetrySource.hpp"
#include "LogManager.hpp"
#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include <nlohmann/json.hpp>


class TelemetryApplication {
    private:
    void loadConfig(const std::string& path);
    void setUpLogManager(void);
    void setUpTelemetrySources(void);
    void telemetryLoop(std::unique_ptr<ITelemetrySource> source, int rateMs);

    nlohmann::json config; /* creates a json object */
    std::unique_ptr<LogManager> logManager;
    std::vector<std::thread> telemetryThreads;
    std::atomic<bool> running{true};
    
public:
  TelemetryApplication(const std::string &configPath);
  ~TelemetryApplication();

  void start(void);
};

#endif