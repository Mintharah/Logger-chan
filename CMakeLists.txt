cmake_minimum_required(VERSION 3.16)

project(Logging
        VERSION 1.0
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_CXX_EXTENTIONS OFF)

add_executable(Logging_app
            src/main.cpp
            src/LogManager.cpp
            src/FileSinkImpl.cpp
            src/ConsoleSinkImpl.cpp
            src/LogMessage.cpp
            src/FileTelemetrySourceImpl.cpp
            src/SafeFile.cpp
            src/CPUTelemetrySource.cpp
            src/MemTelemetrySourceImpl.cpp
            src/Common.cpp
            src/LogSinkFactory.cpp)

target_include_directories(Logging_app PRIVATE ${PROJECT_SOURCE_DIR}/inc)

target_compile_options(Logging_app
    PRIVATE
        -Wall -Wextra -Wpedantic
)


