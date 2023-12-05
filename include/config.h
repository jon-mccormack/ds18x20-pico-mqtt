#ifndef _INCLUDE_CONFIG_H_
#define _INCLUDE_CONFIG_H_

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

#ifndef DPM_SENDOR_ID
#define DPM_SENSOR_ID "1"
#endif


#endif // _INCLUDE_CONFIG_H_