cmake_minimum_required(VERSION 3.16)

project(Logging
        VERSION 1.0
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_library(
    VSOMEIP_LIB
    NAMES vsomeip3
    PATHS "/home/yasmine/Desktop/someip/vsomeip"
    NO_DEFAULT_PATH
    REQUIRED
)

set(VSOMEIP_SRC "/home/yasmine/Desktop/someip/vsomeip")
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(Logging_app
            src/main_client.cpp
            src/LogManager.cpp
            src/FileSinkImpl.cpp
            src/ConsoleSinkImpl.cpp
            src/LogMessage.cpp
            src/FileTelemetrySourceImpl.cpp
            src/SafeFile.cpp
            src/CPUTelemetrySource.cpp
            src/MemTelemetrySourceImpl.cpp
            src/Common.cpp
            src/LogSinkFactory.cpp
            src/SomeIPTelemetrySourceImpl.cpp
            src/TelemetryApplication.cpp)
            
target_include_directories(Logging_app PRIVATE 
    ${PROJECT_SOURCE_DIR}/inc
    ${VSOMEIP_SRC}/interface
)

find_library(BOOST_THREAD NAMES boost_thread PATHS /home/yasmine/rpi_sysroot/usr/lib/aarch64-linux-gnu NO_DEFAULT_PATH)
find_library(BOOST_SYSTEM NAMES boost_system PATHS /home/yasmine/rpi_sysroot/usr/lib/aarch64-linux-gnu NO_DEFAULT_PATH)
find_library(BOOST_FILESYSTEM NAMES boost_filesystem PATHS /home/yasmine/rpi_sysroot/usr/lib/aarch64-linux-gnu NO_DEFAULT_PATH)

target_link_libraries(Logging_app PRIVATE 
    ${VSOMEIP_LIB}
    ${BOOST_THREAD}
    ${BOOST_SYSTEM}
    ${BOOST_FILESYSTEM}
    pthread
)
target_compile_options(Logging_app
    PRIVATE
    -Wall -Wextra -Wpedantic
)


