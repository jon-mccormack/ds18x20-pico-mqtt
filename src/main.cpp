#include <stdio.h>
#include <ostream>
#include "pico/stdlib.h"
#include "one_wire.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "wifi.h"
#include "lwip/apps/mqtt.h"
// #include "lwip/mq"

// TODO: might add pico-sdk as a submodule to this project, makes version pinning easier and 
// means better support for anyone using a non-vscode/docker environment

void callback(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {}

void connectToWifi(const std::string &ssid, const std::string &password)
{
    printf(std::string("Launching using ssid " + ssid + " and password " + password + "\n").c_str());

    if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK))
    {
        throw std::runtime_error("Failed to initialise CYW43 Wifi chip");
    }

    // enable station mode, which allows the pico to connect to access points
    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(DPM_WIFI_SSID, DPM_WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000))
    {
        throw std::runtime_error("Failed to connect to Wifi");
    }
    printf("Connected to Wifi\n");
}

void mqtt()
{
    mqtt_client_t *client = mqtt_client_new();
    struct mqtt_connect_client_info_t ci;
    err_t err;

    /* Setup an empty client info structure */
    memset(&ci, 0, sizeof(ci));

    /* Minimal amount of information required is client identifier, so set it here */
    ci.client_id = "lwip_test";

    /* Initiate client and connect to server, if this fails immediately an error code is returned
       otherwise mqtt_connection_cb will be called with connection result after attempting
       to establish a connection with the server.
       For now MQTT version 3.1.1 is always used */

    ip_addr_t broker;
    IP4_ADDR(&broker, 192, 168, 1, 37);
    err = mqtt_client_connect(client, &broker, 1883, callback, 0, &ci);

    //   /* For now just print the result code if something goes wrong
    if (err != ERR_OK)
    {
        printf("mqtt_connect return %d\n", err);
    }
}

int main()
{
    try
    {
        stdio_init_all();
        // added this to give my usb serial client time to hook into the program's standard output
        sleep_ms(1000);

        // connect to the wifi network
        connectToWifi(DPM_WIFI_SSID, DPM_WIFI_PASSWORD);

        mqtt();

        One_wire one_wire(17);
        one_wire.init();
        rom_address_t address{};
        while (true)
        {
            one_wire.single_device_read_rom(address);
            one_wire.convert_temperature(address, true, false);
            printf("Temperature: %3.1foC\n", one_wire.temperature(address));
            sleep_ms(1000);
        }
    }
    catch (const std::exception &ex)
    {
        printf(std::string("Caught exception: " + std::string(ex.what()) + "/n").c_str());
        return EXIT_FAILURE;
    }
    catch (...)
    {
        printf("Caught unknown exception/n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
