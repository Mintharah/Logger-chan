# Logger-chan

A configurable, multi-sink telemetry logging framework written in modern C++17.  
Supports console/file sinks, telemetry sources (CPU, file, shell script), JSON configuration, and extensible plug-in design.

This project demonstrates:

- Custom logging with severity levels  
- Threaded and asynchronous logging  
- Policy-based formatting  
- Ring buffer with producer/consumer model  
- JSON configuration using JSON for Modern C++  
- Pluggable sinks and telemetry sources  

---

## Features

✔ Multi-sink logging (console, file)  
✔ Multiple telemetry sources (CPU, Shell script, vSOME/IP)  
✔ Ring buffer + threaded consumer (high performance)  
✔ Policy-based formatting  
✔ JSON configuration at runtime  
✔ Extendable design patterns (Facade, Adapter, Builder, Factory)  

---

## Building the Project

### Requirements

- Modern C++ compiler (GCC/Clang) with C++17 support  
- CMake ≥ 3.16  
- nlohmann/json single-header library  
- Optional: vSOME/IP library (if using the SomeIP telemetry source)  

### Build Steps

```bash
git clone https://github.com/Mintharah/Logger-chan.git
cd Logger-chan
mkdir build && cd build
cmake ..
cmake --build .
```
The executable will be:
```
./build/Logging_app
```
### Configuration

Logger-chan uses a config.json file to enable/disable sources, set logging intervals, and define sink behavior.

Example config.json:
```
{
  "logging": {
    "buffer_size": 1024
  },
  "sinks": {
    "console": true,
    "file": { "enabled": true, "path": "/home/yasmine/Desktop/cpu_logs.txt" }
  },
  "telemetry_sources": {
    "cpu": { "enabled": true, "rate_ms": 500 },
    "someip": { "enabled": false, "rate_ms": 1000 }
  }
}
```
### Usage
Run the Logger
```
./build/Logging_app
```
Logger-chan will:
1. Load and parse config.json
2. Initialize sinks and telemetry sources
3. Continuously gather telemetry data
4. Format and route messages to console/file

Example output:
```
[Telemetry App][CPU][INFO][2026-01-19 16:31:36][8.385538 %]
```
### Example Telemetry Input Script
A sample shell script that generates random CPU values for testing:
```
#!/bin/bash

LOG_FILE="/home/nemesis/shell_logs"
DURATION=250
INTERVAL=0.1

: > "$LOG_FILE"
START_TIME=$(date +%s)

while (( $(date +%s) - START_TIME < DURATION )); do
    VALUE=$(awk -v r=$RANDOM 'BEGIN { printf "%.2f", r/32767*100 }')
    echo "$VALUE"
    echo "$VALUE" >> "$LOG_FILE"
    sleep "$INTERVAL"
done
```
### Design Pattern Usage
Pattern	Purpose
Facade:	Simplifies application setup and execution behind a single interface
Builder: Step by step construction of complex objects (LogManager)
Factory: Creates sinks based on configuration
Adapter: Wraps various telemetry sources into a common interface
Producer/Consumer: Ring buffer + worker thread for async logging

### Extending this Project
Add a new telemetry source
1. Create a class inheriting ITelemetrySource
2. Implement openSource() and readSource()
3. Register in TelemetryApplication::setupTelemetrySources

Add a new sink type
1. Create a class implementing ILogSink
2. Override writeMessage(LogMessage&)
3. Update LogManagerBuilder/Factory

### License
Distributed under the MIT License — see LICENSE for details.

### Acknowledgments
This project uses JSON for Modern C++ for configuration parsing and was developed with a focus on extensible systems design and performance-oriented logging mechanics.
