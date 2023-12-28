#ifndef CONFIG_H_
#define CONFIG_H_

// The following should be overridden by CMake build config

#ifndef DPM_WIFI_SSID
#define DPM_WIFI_SSID ""
#endif

#ifndef DPM_WIFI_PASSWORD
#define DPM_WIFI_PASSWORD ""
#endif

#ifndef DPM_MQTT_BROKER_ADDR
#define DPM_MQTT_BROKER_ADDR ""
#endif

#ifndef DPM_MQTT_BROKER_PORT
#define DPM_MQTT_BROKER_PORT 0
#endif

#ifndef DPM_MQTT_PUBLISH_TOPIC
#define DPM_MQTT_PUBLISH_TOPIC ""
#endif

#ifndef DPM_DEVICE_NAME
#define DPM_DEVICE_NAME "DS18x20 Temperature Sensor"
#endif

#ifndef DPM_HA_MQTT_PREFIX
#define DPM_HA_MQTT_PREFIX "homeassistant"
#endif

#ifndef DPM_GPIO_PIN
#define DPM_GPIO_PIN 17
#endif

#endif  // CONFIG_H_
