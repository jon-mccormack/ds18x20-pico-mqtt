#include <stdio.h>
#include "pico/stdlib.h"
#include "one_wire.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "wifi.h"
#include <ostream>

void connectToWifi(const std::string &ssid, const std::string &password)
{
    printf(std::string("Launching using ssid " + ssid + " and password " + password + "\n").c_str());

    if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK))
    {
        throw std::runtime_error("Failed to initialise CYW43 Wifi chip");
    }

    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(DPM_WIFI_SSID, DPM_WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000))
    {
        throw std::runtime_error("Failed to connect to Wifi");
    }
    printf("Connected to Wifi\n");
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
