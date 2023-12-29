# DS18x20 Pico MQTT

An MQTT client for the Pico W that publishes DS18x20 temperature
readings in a Home Assistant-friendly format.

## Features

- Support for the Raspberry Pi Pico W.
- Publishes temperature readings from a DS18x20 sensor in a Home
Assistant-friendly format at a rate of 1Hz.

## Getting Started (Hardware)

TODO(Jon). Some nice pics of the ginished product, and the wiring diagram
would be nice.

## Getting Started (Software)

Ensure Git submodules are resolved.

```bash
git submodule update --recursive --init
```

Install build-time dependencies.

```bash
apt-get install -y cmake python3 gcc-arm-none-eabi g++
```

Now you're ready to start building the project.

## Building

Run the following to build the project, ensuring you update the config options
to suit your setup.

```bash
mkdir build
cd build
cmake \
  -DPICO_SDK_PATH="modules/pico-sdk" \
  -DWIFI_SSID="network ssid" \
  -DWIFI_PASSWORD="network password" \
  -DMQTT_BROKER_ADDRESS="192.168.1.2" \
  -DMQTT_BROKER_PORT=1883 \
  -DDEVICE_NAME="Pico DS18x20 Temperature Probe" \
  -DHA_MQTT_PREFIX="homeassistant" \
  -DGPIO_PIN=17 \
  ..
cmake --build .
```

The binaries will now be available in the `/build` directory.

## Running

To run the project, copy the built `ds18x20-pico-mqtt.uf2` (in the `/build`
directory) binary over to your Pico; ensure your Pico is in USB Mass Storage
mode by holding the `BOOTSEL` button when powering up.

The Pico will run the binary once it restarts.

## Debugging

You can use USB Serial (minicom) to view the standard output of the app
running on the Pico. Run `minicom -b 115200 -o -D /dev/ttyACM0` from the
host that the Pico is connected to and you should start seeing the logs.
Note, minicom must be installed for this to work
(`apt-get install -y minicom`).

## VS Code Dev Container

There are pre-configured `.devcontainer` and `.vscode` directories committed
to this repo which should make setting up the development environment really
easy.

_Note: when building the Dev Container, your Pico must be connected! This is
because the `/dev/ttyACM0` device is forwarded to the container - so creating
the container will fail if its unreachable._

Config options that the CMake build task needs access to are defined in the
`.env.sh.example` file. Duplicate that file with the new name `env.sh` and
enter your config within and it should be used by the built binaries from then
onwards.

Run the VS Code build command (`ctrl+shift+b`) or run the "Build" custom task.
The binaries will now be available in the `/build` directory. You can copy
the built binary over to the Pico in the same way as mentioned above.

You can also run the task "Monitor Pico's USB serial output" to monitor the
standard output of the Pico when its running (and connected!).

## Changelog

See [CHANGELOG.md](/CHANGELOG.md).
