cmake_minimum_required(VERSION 3.22)

SET(PICO_BOARD pico_w)

# Reference Pico CMake functions
include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

# Define project
project(ds18x20-pico-mqtt VERSION 0.1.0)

# Apply project config from Pico
pico_sdk_init()

option(WIFI_SSID "Wifi network's SSID" "some-ssid")
option(WIFI_PASSWORD "Wifi network's password" "some-password")

# target_compile_definitions(${PROJECT_NAME} WIFI_SSID=${WIFI_SSID} WIFI_PASSWORD=${WIFI_PASSWORD})
add_compile_definitions(WIFI_SSID=${WIFI_SSID} WIFI_PASSWORD=${WIFI_PASSWORD})

# Project source files
file (GLOB_RECURSE SRC "src/*.cpp")

# Add source files to project
add_executable(${PROJECT_NAME}
    ${SRC}
)

# Third-party dependencies
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/modules/pico-onewire)

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

# Output stdout over usb serial, not uart
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# Create map/bin/hex/uf2 file in addition to ELF
pico_add_extra_outputs(${PROJECT_NAME})
