cmake_minimum_required(VERSION 3.18)

SET(PICO_BOARD pico_w)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Reference Pico CMake functions
include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

# Define project
project(ds18x20-pico-mqtt VERSION 0.1.0)

# Apply project config from Pico
pico_sdk_init()

# Wifi configuration
option(WIFI_SSID "Wifi network's SSID")
option(WIFI_PASSWORD "Wifi network's password")
add_compile_definitions(DPM_WIFI_SSID="${WIFI_SSID}" DPM_WIFI_PASSWORD="${WIFI_PASSWORD}")
message("Using ${WIFI_SSID} ${WIFI_PASSWORD}")

# Project source files
file (GLOB_RECURSE SRC "src/*.cpp")

# Add source files to project
add_executable(${PROJECT_NAME}
    ${SRC}
    $ENV{PICO_SDK_PATH}/lib/lwip/src/apps/mqtt/mqtt.c
)

# Headers
target_include_directories(${PROJECT_NAME}
    PRIVATE 
        include/
)

# Link libs
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    pico_one_wire
    pico_cyw43_arch_lwip_threadsafe_background
)


# Third-party dependencies
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/modules/pico-onewire)

# Output stdout over usb serial, not uart
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# Create map/bin/hex/uf2 file in addition to ELF
pico_add_extra_outputs(${PROJECT_NAME})
