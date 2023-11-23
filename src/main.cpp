#include <stdio.h>
#include "pico/stdlib.h"
#include "one_wire.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "wifi.h"

int main()
{
    stdio_init_all();

    printf("Wifi ssid and password following:\n");

    // printf(WIFI_SSID);
    // printf(WIFI_PASSWORD);
    // printf("\n");

    if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK))
    {
        printf("failed to initialise\n");
        return 1;
    }
    printf("initialised\n");
    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms("help", "help", CYW43_AUTH_WPA2_AES_PSK, 10000))
    {
        printf("failed to connect\n");
        return 1;
    }
    printf("connected\n");

    One_wire one_wire(17);
    one_wire.init();
    rom_address_t address{};
    while (true)
    {
        one_wire.single_device_read_rom(address);
        printf("Device Address: %02x%02x%02x%02x%02x%02x%02x%02x\n", address.rom[0], address.rom[1], address.rom[2], address.rom[3], address.rom[4], address.rom[5], address.rom[6], address.rom[7]);
        one_wire.convert_temperature(address, true, false);
        printf("Temperature: %3.1foC\n", one_wire.temperature(address));
        sleep_ms(1000);
    }
    return 0;
}
