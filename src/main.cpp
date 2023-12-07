#include <stdio.h>
#include <ostream>
#include <iostream>
#include <sstream>
#include "pico/stdlib.h"
#include "one_wire.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "config.h"
#include "lwip/apps/mqtt.h"
#include <algorithm>
#include "lwip/arch.h"
#include <sstream>
#include "MqttClient.h"

void connectToWifi(const std::string &ssid, const std::string &password)
{
    std::cout << "Connecting to Wifi using ssid " << ssid << " and password " << password << std::endl;
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK))
    {
        throw std::runtime_error("Failed to initialise CYW43 Wifi chip");
    }

    // enable station mode, which allows the pico to connect to access points
    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(ssid.c_str(), password.c_str(), CYW43_AUTH_WPA2_AES_PSK, 10000))
    {
        throw std::runtime_error("Failed to connect to Wifi");
    }
    std::cout << "Connected to Wifi!" << std::endl;
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
         << "\"device_class\":\"temperature\""
         << "}";

    return json.str();
}

int main()
{
    try
    {
        stdio_init_all();
        // added this to give my usb serial client time to hook into the program's standard output
        sleep_ms(5000);

        // connect to the wifi network
        connectToWifi(DPM_WIFI_SSID, DPM_WIFI_PASSWORD);

        MqttClient client(DPM_MQTT_BROKER_ADDR, DPM_MQTT_BROKER_PORT, "pico-ds18x20-temp-sensor");

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
            sleep_ms(1000);
        }
    }
    catch (const std::exception &ex)
    {
        std::cout << "Caught exception: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cout << "Caught unknown exception..." << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
