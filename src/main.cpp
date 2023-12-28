#include <stdio.h>

#include <algorithm>
#include <iostream>
#include <ostream>
#include <sstream>

#include "DS18x20.h"
#include "MqttClient.h"
#include "config.h"
#include "hardware/watchdog.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

void connectToWifi(const std::string &ssid, const std::string &password) {
  // 60 second timeout
  int res = cyw43_arch_wifi_connect_timeout_ms(ssid.c_str(), password.c_str(), CYW43_AUTH_WPA2_AES_PSK, 60000);
  if (res != pico_error_codes::PICO_OK) {
    throw std::runtime_error("Failed to connect to Wifi, connect response: " + std::to_string(res));
  }
}

int main() {
  stdio_init_all();
  sleep_ms(5000);
  std::cout << std::endl;

  if (watchdog_caused_reboot()) {
    std::cout << "Rebooted by Watchdog" << std::endl;
  } else {
    std::cout << "Clean boot" << std::endl;
  }

  try {
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK) != pico_error_codes::PICO_OK) {
      std::cerr << "Failed to init CYW43 with UK Country" << std::endl;
      return 1;
    }
    std::cout << "CYW43 initialised" << std::endl;

    // enable station mode, which sets this cyw43 chip as a client to connect to
    // other aps
    cyw43_arch_enable_sta_mode();
    std::cout << "Station mode enabled" << std::endl;

    std::cout << "Connecting to Wifi using ssid " << DPM_WIFI_SSID << " and password " << DPM_WIFI_PASSWORD
              << std::endl;
    connectToWifi(DPM_WIFI_SSID, DPM_WIFI_PASSWORD);
    std::cout << "Connected to Wifi" << std::endl;

    // enable watchdog, so if for any reason the program fails to check-in with
    // the watchdog within 8 seconds, the watchdog will reboot the Pico
    watchdog_enable(8000, false);
    std::cout << "Watchdog enabled" << std::endl;

    // TODO(Jon): fix mqtt client Id.. need to get the unique device Id from the sensor.. perhaps pull that func into
    // main?
    MqttClient mqttc(DPM_MQTT_BROKER_ADDR, DPM_MQTT_BROKER_PORT, "test-client-id");
    DS18x20 sensor(&mqttc, DPM_DEVICE_NAME, DPM_HA_MQTT_PREFIX, DPM_GPIO_PIN);

    // will block indefinitely
    sensor.monitor();
  } catch (const std::exception &ex) {
    std::cerr << "Caught exception: " << ex.what() << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception..." << std::endl;
  }

  std::cout << "Rebooting via Watchdog." << std::endl;

  watchdog_reboot(0, 0, 0);
}
