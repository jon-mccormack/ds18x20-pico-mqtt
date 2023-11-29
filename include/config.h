#ifndef _INCLUDE_CONFIG_H_
#define _INCLUDE_CONFIG_H_

// should be overridden by CMake build config

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


#endif // _INCLUDE_CONFIG_H_