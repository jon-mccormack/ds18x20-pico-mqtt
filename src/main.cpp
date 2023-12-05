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
#include "lwip/arch.h"
#include "MqttClient.h"
#include "HAMqttDevice.h"

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

std::string getStateJson(float temperature);

int main()
{
    try
    {
        stdio_init_all();
        // added this to give my usb serial client time to hook into the program's standard output
        sleep_ms(5000);

        // connect to the wifi network
        connectToWifi(DPM_WIFI_SSID, DPM_WIFI_PASSWORD);

        MqttClient client(DPM_MQTT_BROKER_ADDR, DPM_MQTT_BROKER_PORT);

        // TODO(Jon): do this https://github.com/plapointe6/HAMqttDevice
        HAMqttDevice tempSensor("Pico DS18x20 Temperature Probe", HAMqttDevice::DeviceType::SENSOR, "homeassistant");

        tempSensor.addConfigVar("unit_of_measurement", "°C");
        tempSensor.addConfigVar("value_template", " {{ value_json.temperature }} ");
        tempSensor.addConfigVar("device_class", "temperature");

        std::cout << "Config topic: " << tempSensor.getConfigTopic() << std::endl;
        std::cout << "Config payload: " << tempSensor.getConfigPayload() << std::endl;
        std::cout << "State topic: " << tempSensor.getStateTopic() << std::endl;
        client.publish(tempSensor.getConfigTopic(), tempSensor.getConfigPayload());

        sleep_ms(3000);

        One_wire one_wire(17);
        one_wire.init();
        rom_address_t address{};
        while (true)
        {
            one_wire.single_device_read_rom(address);
            one_wire.convert_temperature(address, true, false);
            float temp = one_wire.temperature(address);
            if (temp != One_wire::invalid_conversion)
            {
                client.publish(tempSensor.getStateTopic(), getStateJson(temp));
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
