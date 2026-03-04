set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(TOOLCHAIN_PATH "/home/yasmine/x-tools/aarch64-rpi3-linux-gnu/bin")

set(CMAKE_C_COMPILER   ${TOOLCHAIN_PATH}/aarch64-rpi3-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/aarch64-rpi3-linux-gnu-g++)

# 'BOTH' allows CMake to find headers in the local vsomeip source folder
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)