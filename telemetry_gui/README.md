# TelemetryLogger — Server GUI 🌸

Pastel-pink Qt5 dashboard for the **laptop-side SOME/IP server**.

## Architecture

```
Laptop (x86)                          Raspberry Pi (aarch64)
─────────────────────────────         ──────────────────────────────
TelemetryLoggerGUI                    Logging_app  (existing project)
  │                                     │
  │  GUIServer (std::thread)            │  SomeIPTelemetrySourceImpl
  │    vsomeip "Telemetry_Server"  ◄────┤    requests CPU data
  │    reads /proc/stat                 │
  │    LogManager                       │  LogManager
  │      ConsoleSinkImpl                │    ConsoleSinkImpl
  │      FileSinkImpl (opt)             │    FileSinkImpl
  │      GUISink  ──────────────────►  │
  │        Qt signal (queued)           │
  │    MainWindow shows live data       │
```

## Build

```bash
# Install Qt5 dev tools if not already present
sudo apt install qtbase5-dev cmake build-essential

# From Linux_Project/
mkdir telemetry_gui/build && cd telemetry_gui/build
cmake ..
make -j$(nproc)
```

## Run

```bash
# Point vsomeip at the server config (or set the path in the GUI sidebar)
export VSOMEIP_CONFIGURATION=../Server/server.json

./TelemetryLoggerGUI
```

Press **▶ Start Server** — the GUI launches the real SOME/IP server,
reads `/proc/stat` on every client request, logs through `LogManager`,
and streams every `LogMessage` live into the pink dashboard table.

## What each GUI element shows

| Element | Real data source |
|---|---|
| Log table | Every `LogMessage` from `LogFormatter<CpuPolicy>` via `GUISink` |
| Ring buffer bar | `LogManager::bufferSize()` polled every 500 ms |
| Total CPU reads | Atomic counter incremented per `log()` call |
| Latest CPU % | Parsed from `msg.text` (e.g. `"34.200001 %"`) |
| Critical / Warning counts | Counted from real severity field |
