#ifndef DS81X20_H_
#define DS81X20_H_

#include <string>

#include "MqttClient.h"

class DS18x20 {
 private:
  MqttClient *mqttClient;

 public:
  explicit DS18x20(MqttClient *mqttClient);
  void monitor();
};

#endif  // DS81X20_H_
