#ifndef DS18X20_H_
#define DS18X20_H_

#include <string>

#include "MqttClient.h"

class DS18x20 {
 private:
  MqttClient* p_mqttClient;
  const std::string m_deviceName;
  const std::string m_deviceId;
  const std::string m_haMqttPrefix;
  uint m_gpioPin;

  void publishConfigPayload();
  void publishStatePayload(float temperature);

 public:
  explicit DS18x20(MqttClient* mqttClient, const std::string& deviceName, const std::string& haMqttPrefix,
                   uint gpioPin);
  void monitor();
};

#endif  // DS18X20_H_
