#ifndef _INCLUDE_TEMPERATUREPUBLISHER_
#define _INCLUDE_TEMPERATUREPUBLISHER_

#include <string>
#include "lwip/apps/mqtt.h"

class MqttClient
{
private:
    mqtt_client_t *client;

public:
    MqttClient(const std::string &address, uint16_t port, const std::string& clientId);
    void publish(const std::string &topic, const std::string &payload);
    void publishDiscoveryMessage(const std::string &sensorId);
    void publishStateMessage(const std::string &sensorId, float temperature);
};

#endif