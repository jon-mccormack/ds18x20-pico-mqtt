cmake_minimum_required(VERSION 3.22)

# Reference Pico CMake functions
include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

# Define project
project(ds18x20-pico-mqtt VERSION 0.1.0)

# Apply project config from Pico
pico_sdk_init()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/modules/pico-onewire)

# Project source files
file (GLOB_RECURSE SRC "src/*.cpp")

# Add source files to project
add_executable(${PROJECT_NAME}
    ${SRC}
)


# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    pico_one_wire
)

# Output stdout over usb serial
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)


# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})
