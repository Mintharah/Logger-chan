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
