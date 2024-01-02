#include "DS18x20.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include "hardware/gpio.h"
#include "hardware/watchdog.h"
#include "one_wire.h"

std::string getConfigTopic(const std::string &deviceId, const std::string &haMqttPrefix) {
  return haMqttPrefix + "/sensor/" + deviceId + "/config";
}

std::string getStateTopic(const std::string &deviceId, const std::string &haMqttPrefix) {
  return haMqttPrefix + "/sensor/" + deviceId + "/state";
}

std::string getConfigPayload(const std::string &deviceName, const std::string &deviceId,
                             const std::string &haMqttPrefix) {
  std::stringstream json;

  json << "{"
       << "\"~\":\"" << haMqttPrefix << "/sensor/" << deviceId << "\","
       << "\"name\":\"" << deviceName << "\","
       << "\"unique_id\":\"" << deviceId << "\","
       << "\"stat_t\":\"~/state\","
       << "\"unit_of_measurement\":\"°C\","
       << "\"value_template\":\"{{ value_json.temperature }}\","
       << "\"device_class\":\"temperature\","
       << "\"state_class\": \"measurement\""
       << "}";

  return json.str();
}

std::string getStatePayload(float temperature) {
  std::stringstream str;
  str << "{"
      << "\"temperature\": " + std::to_string(temperature) << "}";

  return str.str();
}

void DS18x20::publishConfigPayload() {
  p_mqttClient->publish(getConfigTopic(m_deviceId, m_haMqttPrefix),
                        getConfigPayload(m_deviceName, m_deviceId, m_haMqttPrefix));
}

void DS18x20::publishStatePayload(float temperature) {
  p_mqttClient->publish(getStateTopic(m_deviceId, m_haMqttPrefix), getStatePayload(temperature));
}

DS18x20::DS18x20(MqttClient *mqttClient, const std::string &deviceName, const std::string &deviceId,
                 const std::string &haMqttPrefix, uint gpioPin)
    : p_mqttClient(mqttClient),
      m_deviceName(deviceName),
      m_deviceId(deviceId),
      m_haMqttPrefix(haMqttPrefix),
      m_gpioPin(gpioPin) {
  std::cout << "DS18x20 initialised with Device name: " << m_deviceName << ", Device Id: " << m_deviceId
            << ", HA Mqtt Prefix: " << m_haMqttPrefix << ", GPIO Pin: " << std::to_string(m_gpioPin) << std::endl;
}

void DS18x20::monitor() {
  // advertise to HA the sensor's presence
  publishConfigPayload();

  One_wire one_wire(m_gpioPin);
  one_wire.init();
  rom_address_t address{};
  one_wire.single_device_read_rom(address);
  one_wire.convert_temperature(address, true, false);
  float temp = one_wire.temperature(address);

  // rudimentary way of filtering out 85 c on startup, could be improved really
  while (temp == 85) {
    sleep_ms(100);
    one_wire.single_device_read_rom(address);
    one_wire.convert_temperature(address, true, false);
    temp = one_wire.temperature(address);
  }

  while (true) {
    one_wire.single_device_read_rom(address);
    one_wire.convert_temperature(address, true, false);
    temp = one_wire.temperature(address);
    if (temp != One_wire::invalid_conversion) {
      p_mqttClient->publish(getStateTopic(m_deviceId, m_haMqttPrefix), getStatePayload(temp));
    } else {
      std::cerr << "Could not read temperature from ds18x20!" << std::endl;
    }
    // should de-couple watchdog from this class! could pass a callback to to DS18x20's constructor to call instead
    watchdog_update();
    sleep_ms(1000);
  }
}
