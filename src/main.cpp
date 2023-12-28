#include <stdio.h>
#include <ostream>
#include <iostream>
#include <sstream>
#include "pico/stdlib.h"
#include "one_wire.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "hardware/watchdog.h"
#include "config.h"
#include "lwip/apps/mqtt.h"
#include <algorithm>
#include "lwip/arch.h"
#include <sstream>
#include "MqttClient.h"

void connectToWifi(const std::string &ssid, const std::string &password)
{
    // 60 second timeout
    int res = cyw43_arch_wifi_connect_timeout_ms(ssid.c_str(), password.c_str(), CYW43_AUTH_WPA2_AES_PSK, 60000);
    if (res != pico_error_codes::PICO_OK)
    {
        throw std::runtime_error("Failed to connect to Wifi, connect response: " + std::to_string(res));
    }
}

std::string getStateJson(float temperature)
{
    std::stringstream str;
    str << "{"
        << "\"temperature\": " + std::to_string(temperature)
        << "}";

    return str.str();
}

std::string getUniqueId(const std::string &name)
{
    std::string ret = name;
    std::transform(ret.begin(), ret.end(), ret.begin(),
                   [](unsigned char c)
                   {
                       unsigned char space = ' ';
                       unsigned char underscore = '_';
                       if (c == space)
                       {
                           return underscore;
                       }
                       unsigned char lower = std::tolower(c);
                       return lower;
                   });
    return ret;
}

std::string getConfigTopic(const std::string &name, const std::string &haMqttPrefix)
{
    return haMqttPrefix + "/sensor/" + getUniqueId(name) + "/config";
}

std::string getStateTopic(const std::string &name, const std::string &haMqttPrefix)
{
    return haMqttPrefix + "/sensor/" + getUniqueId(name) + "/state";
}

std::string getConfigPayload(const std::string &name, const std::string &haMqttPrefix)
{
    std::stringstream json;
    std::string uniqueId = getUniqueId(name);

    json << "{"
         << "\"~\":\"" << haMqttPrefix << "/sensor/" << uniqueId << "\","
         << "\"name\":\"" << name << "\","
         << "\"unique_id\":\"" << uniqueId << "\","
         << "\"stat_t\":\"~/state\","
         << "\"unit_of_measurement\":\"°C\","
         << "\"value_template\":\"{{ value_json.temperature }}\","
         << "\"device_class\":\"temperature\","
         << "\"state_class\": \"measurement\""
         << "}";

    return json.str();
}

void monitorTemperature(const std::string &wifiSsid, const std::string wifiPassword, const std::string &mqttBrokerAddress, uint16_t mqttBrokerPort)
{
    MqttClient client(mqttBrokerAddress, mqttBrokerPort, "pico-ds18x20-temp-sensor");

    std::string name = "Pico DS18x20 Temperature Probe";
    std::string haMqttPrefix = "homeassistant";

    client.publish(getConfigTopic(name, haMqttPrefix), getConfigPayload(name, haMqttPrefix));

    One_wire one_wire(17);
    one_wire.init();
    rom_address_t address{};
    one_wire.single_device_read_rom(address);
    one_wire.convert_temperature(address, true, false);
    float temp = one_wire.temperature(address);

    // rudimentary way of filtering out 85 c on startup
    while (temp == 85)
    {
        std::cout << "Reading 85!!" << std::endl;
        sleep_ms(100);
        one_wire.single_device_read_rom(address);
        one_wire.convert_temperature(address, true, false);
        temp = one_wire.temperature(address);
    }

    while (true)
    {
        one_wire.single_device_read_rom(address);
        one_wire.convert_temperature(address, true, false);
        temp = one_wire.temperature(address);
        if (temp != One_wire::invalid_conversion)
        {
            client.publish(getStateTopic(name, haMqttPrefix), getStateJson(temp));
        }
        else
        {
            std::cerr << "Could not read temperature from ds18x20!" << std::endl;
        }
        watchdog_update();
        sleep_ms(1000);
    }
}

int main()
{
    stdio_init_all();
    sleep_ms(5000);

    if (watchdog_caused_reboot())
    {
        std::cout << "Rebooted by Watchdog" << std::endl;
    }
    else
    {
        std::cout << "Clean boot" << std::endl;
    }

    try
    {
        if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK) != pico_error_codes::PICO_OK)
        {
            std::cerr << "Failed to init CYW43 with UK Country" << std::endl;
            return 1;
        }
        std::cout << "CYW43 initialised" << std::endl;

        // enable station mode, which sets this cyw43 chip as a client to connect to other
        // aps
        cyw43_arch_enable_sta_mode();
        std::cout << "Station mode enabled" << std::endl;

        std::cout << "Connecting to Wifi using ssid " << DPM_WIFI_SSID << " and password " << DPM_WIFI_PASSWORD << std::endl;
        connectToWifi(DPM_WIFI_SSID, DPM_WIFI_PASSWORD);
        std::cout << "Connected to Wifi" << std::endl;

        // enable watchdog, so if for any reason the program fails to check-in with the watchdog within
        // 8 seconds, the watchdog will reboot the Pico
        watchdog_enable(8000, false);
        std::cout << "Watchdog enabled" << std::endl;

        // begin publishing temps!
        monitorTemperature(DPM_WIFI_SSID, DPM_WIFI_PASSWORD, DPM_MQTT_BROKER_ADDR, DPM_MQTT_BROKER_PORT);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Caught exception: " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception..." << std::endl;
    };

    std::cout << "Rebooting via Watchdog." << std::endl;

    watchdog_reboot(0, 0, 0);
}
