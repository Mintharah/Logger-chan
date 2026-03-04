# Logger-chan 🌸

A C++ logging and telemetry system featuring a server backend and a GUI frontend for real-time data monitoring and file-based log management.

---

## 📁 Project Structure

```
Logger-chan/
├── Server/             # Server-side logging backend
├── inc/                # Header files
├── src/                # Source files
├── telemetry_gui/      # GUI frontend for telemetry visualization
├── local_config/       # Local configuration overrides
├── CMakeLists.txt      # CMake build configuration
├── toolchain.cmake     # Cross-compilation toolchain settings
├── config.json         # Main configuration file
├── client.json         # Client connection settings
├── shell_log_src.sh    # Shell script for log source management
└── FileLogging.txt     # File logging documentation/notes
```

---

## 🔧 Requirements

- C++17 or later
- CMake 3.15+
- A compatible C++ compiler (GCC, Clang, MSVC)

---

## 🚀 Build

```bash
# Clone the repository
git clone https://github.com/Mintharah/Logger-chan.git
cd Logger-chan

# Create build directory
mkdir build && cd build

# Configure
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake

# Build
cmake --build .
```

> If not cross-compiling, you can omit the `-DCMAKE_TOOLCHAIN_FILE` flag:
> ```bash
> cmake ..
> cmake --build .
> ```

---

## ⚙️ Configuration

Edit `config.json` to set up the logging server parameters:

```json
{
  "host": "127.0.0.1",
  "port": 8080,
  "log_level": "INFO",
  "log_dir": "./logs"
}
```

Edit `client.json` to configure the client connection:

```json
{
  "server_host": "127.0.0.1",
  "server_port": 8080
}
```

> Note: Adjust these to match your actual `config.json` / `client.json` fields.

---

## 🖥️ Running

**Start the server:**
```bash
./build/Server/logger_server
```

**Launch the telemetry GUI:**
```bash
./build/telemetry_gui/telemetry_gui
```

**Using the shell helper:**
```bash
bash shell_log_src.sh
```

---

## 📄 File Logging

See [`FileLogging.txt`](./FileLogging.txt) for details on how log files are structured and rotated.

---

## 🤝 Contributing

1. Fork the repo
2. Create a feature branch (`git checkout -b feature/my-feature`)
3. Commit your changes (`git commit -m 'Add my feature'`)
4. Push to the branch (`git push origin feature/my-feature`)
5. Open a Pull Request

---

## 📜 License

This project does not currently specify a license. Please contact the author before using it in production.
