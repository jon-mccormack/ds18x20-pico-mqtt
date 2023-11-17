#include <stdio.h>
#include "pico/stdlib.h"
#include "one_wire.h"
#include "hardware/gpio.h"

// TODO: I want a vscode task which can build the project

int main()
{
    stdio_init_all();
    One_wire one_wire(17); // GP15 - Pin 20 on Pi Pico
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
