cmake_minimum_required(VERSION 3.18)

set(PICO_BOARD pico_w)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Reference Pico CMake config
if (DEFINED ENV{PICO_SDK_PATH})
    SET (PICO_SDK_PATH "$ENV{PICO_SDK_PATH}")
endif()
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

# Define project
project(ds18x20-pico-mqtt VERSION 0.1.0 LANGUAGES CXX C ASM)

# Apply project config from Pico
pico_sdk_init()

# Wifi configuration
option(WIFI_SSID "Wifi network's SSID")
option(WIFI_PASSWORD "Wifi network's password")

# Not really happy with this config solution, but it works
if (DEFINED ENV{WIFI_SSID})
    set(WIFI_SSID "$ENV{WIFI_SSID}")
endif()
if (DEFINED ENV{WIFI_PASSWORD})
    set(WIFI_PASSWORD "$ENV{WIFI_PASSWORD}")
endif()
if (DEFINED ENV{MQTT_BROKER_ADDRESS})
    set(MQTT_BROKER_ADDRESS "$ENV{MQTT_BROKER_ADDRESS}")
endif()
if (DEFINED ENV{MQTT_BROKER_PORT})
    set(MQTT_BROKER_PORT "$ENV{MQTT_BROKER_PORT}")
endif()
if (DEFINED ENV{DEVICE_NAME})
    set(DEVICE_NAME "$ENV{DEVICE_NAME}")
endif()
if (DEFINED ENV{HA_MQTT_PREFIX})
    set(HA_MQTT_PREFIX "$ENV{HA_MQTT_PREFIX}")
endif()
if (DEFINED ENV{GPIO_PIN})
    set(GPIO_PIN "$ENV{GPIO_PIN}")
endif()

# Build-time macro definition
add_compile_definitions(
    DPM_WIFI_SSID="${WIFI_SSID}"
    DPM_WIFI_PASSWORD="${WIFI_PASSWORD}"
    DPM_MQTT_BROKER_ADDR="${MQTT_BROKER_ADDRESS}"
    DPM_MQTT_BROKER_PORT=${MQTT_BROKER_PORT}
    DPM_DEVICE_NAME="${DEVICE_NAME}"
    DPM_HA_MQTT_PREFIX="${HA_MQTT_PREFIX}"
    DPM_GPIO_PIN=${GPIO_PIN}
)

# Project source files
file (GLOB_RECURSE SRC "src/*.cpp")

# Add source files to project
add_executable(${PROJECT_NAME}
    ${SRC}
    ${PICO_SDK_PATH}/lib/lwip/src/apps/mqtt/mqtt.c
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
