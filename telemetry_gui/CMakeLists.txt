cmake_minimum_required(VERSION 3.16)
project(TelemetryLoggerGUI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# AUTOMOC is disabled — we call qt5_wrap_cpp explicitly below to guarantee
# that moc runs on every Q_OBJECT header, with no silent misses.
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ── Qt5 ───────────────────────────────────────────────────────────────────────
find_package(Qt5 REQUIRED COMPONENTS Widgets Core)

# ── vsomeip3 — the x86 build at /usr/local/lib (same one Server/CMakeLists uses)
find_library(VSOMEIP_LIB NAMES vsomeip3 PATHS /usr/local/lib REQUIRED)

# ── Paths ─────────────────────────────────────────────────────────────────────
set(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
get_filename_component(PROJECT_ROOT "${PROJECT_ROOT}" ABSOLUTE)
message(STATUS "Project root: ${PROJECT_ROOT}")

set(VSOMEIP_SRC "${PROJECT_ROOT}/../someip/vsomeip")
message(STATUS "vsomeip headers: ${VSOMEIP_SRC}/interface")

# ── Include dirs ──────────────────────────────────────────────────────────────
# telemetry_gui/inc is listed FIRST so its LogManager.hpp shadows the original
set(GUI_INCLUDES
    inc
    ${PROJECT_ROOT}/inc
    ${VSOMEIP_SRC}/interface
)

# ── Explicit MOC generation ───────────────────────────────────────────────────
# qt5_wrap_cpp guarantees moc runs — no silent misses unlike AUTOMOC.
qt5_wrap_cpp(MOC_SRCS
    inc/GUISink.hpp
    inc/GUIServer.hpp
    inc/MainWindow.hpp
    OPTIONS
        -I${CMAKE_CURRENT_SOURCE_DIR}/inc
        -I${PROJECT_ROOT}/inc
        -I${VSOMEIP_SRC}/interface
)

# ── Application sources ───────────────────────────────────────────────────────
set(PROJECT_SOURCES
    ${PROJECT_ROOT}/src/CPUTelemetrySource.cpp
    ${PROJECT_ROOT}/src/SafeFile.cpp
    ${PROJECT_ROOT}/src/LogManager.cpp
    ${PROJECT_ROOT}/src/LogMessage.cpp
    ${PROJECT_ROOT}/src/Common.cpp
    ${PROJECT_ROOT}/src/FileSinkImpl.cpp
    ${PROJECT_ROOT}/src/ConsoleSinkImpl.cpp
)

set(GUI_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/GUISink.cpp
    src/GUIServer.cpp
)

# ── Executable ────────────────────────────────────────────────────────────────
add_executable(TelemetryLoggerGUI ${GUI_SOURCES} ${MOC_SRCS} ${PROJECT_SOURCES})
target_include_directories(TelemetryLoggerGUI PRIVATE ${GUI_INCLUDES})
target_link_libraries(TelemetryLoggerGUI PRIVATE Qt5::Widgets Qt5::Core ${VSOMEIP_LIB} pthread)
target_compile_options(TelemetryLoggerGUI PRIVATE -Wall -Wextra)
