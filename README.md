# DS18x20 Pico MQTT

An Mqtt client for the Pico that publishes DS18x20 temperature
readings in a Home Assistant-friendly format.

## Setup

After cloning this repo, ensure that git submodules are also resolved
by running `git submodule update --recursive --init`.

### VS Code Dev Container

When building the dev container, your Pico must be connected! This is because
the `/dev/ttyACM0` device is forwarded to the container - so creating the container
will fail if its unreachable.

## Building

If using VS Code, you should first define your Wifi configuration in
`.vscode/settings.json` in the following way:

```json
{
    "terminal.integrated.env.linux": {
        "WIFI_SSID": "<ssid>",
        "WIFI_PASSWORD": "<password>"
    }
}
```

Then, you can run the "Configure CMake" task, followed by the
"Build" task which should create the binaries.

Otherwise, run the following, ensuring to emplace your Wifi's `ssid`
and `password`.

```bash
rm -rf build # Cleanup build dir if it already exists
mkdir build
cd build
cmake -DWIFI_SSID=<ssid> -DWIFI_PASSWORD=<password> ..
cmake --build .
```

## Running

To run the project, copy the built `ds18x20-pico-mqtt.uf2` file over to your Pico;
ensure your Pico is in USB Mass Storage mode by holding the BOOTSEL button when
powering up.

Once copied, the Pico will run our applicaiton.

## Debugging

To view standard output of our applicaiton whilst its running on the Pico,
we must monitor it over USB serial.

Run the VS Code task "Monitor Pico's USB serial output" or run
`minicom -b 115200 -o -D /dev/ttyACM0` from the host. Note, minicom must be
installed for this to work (`sudo apt-get install minicom`).
