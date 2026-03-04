cmake_minimum_required(VERSION 3.16)
project(Logging VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(VSOMEIP_LIB NAMES vsomeip3 PATHS /usr/local/lib REQUIRED)

add_executable(Logging_app
    SOMEIP_Server.cpp
    ../src/CPUTelemetrySource.cpp
    ../src/SafeFile.cpp
)

target_include_directories(Logging_app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
)

target_link_libraries(Logging_app PRIVATE ${VSOMEIP_LIB} pthread)

target_compile_options(Logging_app PRIVATE -Wall -Wextra -Wpedantic)