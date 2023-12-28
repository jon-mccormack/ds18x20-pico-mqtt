#include "MqttClient.h"

#include <iostream>
#include <sstream>

#include "config.h"
#include "lwip/arch.h"
#include "pico/cyw43_arch.h"

// TODO(Jon): i'm not sure how useful these LWIP-specific macro-function calls are, maybe replace with std::ignore and
// std::cout?

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
  const struct mqtt_connect_client_info_t *client_info = (const struct mqtt_connect_client_info_t *)arg;
  LWIP_UNUSED_ARG(data);

  LWIP_PLATFORM_DIAG(("MQTT client \"%s\" data cb: len %d, flags %d\n", client_info->client_id, static_cast<int>(len),
                      static_cast<int>(flags)));
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len) {
  const struct mqtt_connect_client_info_t *client_info = (const struct mqtt_connect_client_info_t *)arg;
  LWIP_PLATFORM_DIAG(
      ("MQTT client \"%s\" publish cb: topic %s, len %d\n", client_info->client_id, topic, static_cast<int>(tot_len)));
}

void mqtt_request_cb(void *arg, err_t err) {
  const struct mqtt_connect_client_info_t *client_info = (const struct mqtt_connect_client_info_t *)arg;
  LWIP_PLATFORM_DIAG(("MQTT client \"%s\" request cb: err %d\n", client_info->client_id, static_cast<int>(err)));
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
  const struct mqtt_connect_client_info_t *client_info = (const struct mqtt_connect_client_info_t *)arg;
  LWIP_UNUSED_ARG(client);

  // this gets called when we lose connection to the MQTT client! any subsequent publish calls will return a fail
  // (non-zero) status
  LWIP_PLATFORM_DIAG(
      ("MQTT client \"%s\" connection cb: status %d\n", client_info->client_id, static_cast<int>(status)));

  if (status != MQTT_CONNECT_ACCEPTED) {
    std::cerr << "Mqtt connection not accepted :(" << std::endl;
  }

  if (status == MQTT_CONNECT_ACCEPTED) {
    std::cout << "Mqtt connection accepted :)" << std::endl;
  }
}

void mqtt_pub_request_cb(void *arg, err_t err) {
  switch (err) {
    case ERR_OK:
      std::cout << "Publish request callback OK" << std::endl;
      break;
    case ERR_TIMEOUT:
      std::cout << "Publish request callback TIMEOUT" << std::endl;
      break;
    case ERR_ABRT:
      std::cout << "Publish request callback ABRT" << std::endl;
      break;
    default:
      std::cout << "Publish request callback UNKNOWN" << std::endl;
  }
}

ip_addr_t getAddress(const std::string &ip) {
  std::stringstream s(ip);
  int a, b, c, d;  // to store the 4 ints
  char ch;         // to temporarily store the '.'
  s >> a >> ch >> b >> ch >> c >> ch >> d;
  ip_addr_t broker;
  IP4_ADDR(&broker, a, b, c, d);
  return broker;
}

MqttClient::MqttClient(const std::string &address, uint16_t port, const std::string &clientId) {
  std::cout << "Mqtt broker ip: " << address << ", port: " << std::to_string(port) << std::endl;
  cyw43_arch_lwip_begin();
  client = mqtt_client_new();

  static const struct mqtt_connect_client_info_t mqtt_client_info = {clientId.c_str(),
                                                                     NULL, /* user */
                                                                     NULL, /* pass */
                                                                     5,    /* keep alive */
                                                                     NULL, /* will_topic */
                                                                     NULL, /* will_msg */
                                                                     0,    /* will_qos */
                                                                     0 /* will_retain */};

  ip_addr_t broker = getAddress(address);

  mqtt_client_connect(client, &broker, port, mqtt_connection_cb, LWIP_CONST_CAST(void *, &mqtt_client_info),
                      &mqtt_client_info);

  cyw43_arch_lwip_end();
}

void MqttClient::publish(const std::string &topic, const std::string &payload) {
  cyw43_arch_lwip_begin();
  u8_t qos = 1;
  u8_t retain = 0;
  err_t err =
      mqtt_publish(client, topic.c_str(), payload.c_str(), payload.length(), qos, retain, mqtt_pub_request_cb, nullptr);
  cyw43_arch_lwip_end();
  if (err != ERR_OK) {
    throw std::runtime_error("Publish error: " + std::to_string(err));
  }
}
