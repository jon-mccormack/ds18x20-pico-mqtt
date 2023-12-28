#ifndef MQTTCLIENT_H_
#define MQTTCLIENT_H_

#include <string>

#include "lwip/apps/mqtt.h"

class MqttClient {
 private:
  mqtt_client_t *client;

 public:
  MqttClient(const std::string &address, uint16_t port, const std::string &clientId);
  void publish(const std::string &topic, const std::string &payload);
};

#endif  // MQTTCLIENT_H_
