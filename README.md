# DS18x20 Pico MQTT

An Mqtt client for the Pico that publishes DS18x20 temperature
readings in a Home Assistant-friendly format.

## Getting Started

Firstly you should ensure the git submodules are resolved by running the
following.

```bash
git submodule update --recursive --init
```

Now you're ready to start building the project. You can follow either the native
(host) approach, or use VS Code running Dev Containers.

### Pico SDK

TODO(Jon): steps for installing Pico SDK, and reference how the PICO_SDK_PATH
is necessary for the project to build.

## Building

### Natively

There are some additional pre-requisites that you may not already have installed
on your host to build the project. You can install them with the following.

```bash
sudo apt-get update
sudo apt-get install -y build-essential <TODO>
```

Now you can build the project. Make sure you emplace the config options
`<ssid>` and `<password>` to suit your setup.

```bash
mkdir build
cd build
cmake -DWIFI_SSID=<ssid> -DWIFI_PASSWORD=<password> ..
cmake --build .
```

The binaries will now be available in the `/build` directory.

### VS Code Dev Containers

_Note: when building the dev container, your Pico must be connected! This is because
the `/dev/ttyACM0` device is forwarded to the container - so creating the container
will fail if its unreachable._

Make a copy of the `config.sh.example` file (named `config.sh`) and fill in the
environment variables with suitable config. This file is sourced when setting
up the CMake build.

Run the VS Code build command (`ctrl+shift+b`) or run the "Build" custom task.
The binaries will now be available in the `/build` directory.

## Running

To run the project, copy the built `ds18x20-pico-mqtt.uf2` (in the `/build` directory)
file over to your Pico; ensure your Pico is in USB Mass Storage mode by holding the
BOOTSEL button when powering up.

Once copied, the Pico will run our applicaiton.

## Debugging

You can use USB Serial (minicom) to view the standard output of our program
running on the Pico.

Run `minicom -b 115200 -o -D /dev/ttyACM0` from the host that the Pico is
plugged into. Note, minicom must be installed for this to work
(`sudo apt-get install minicom`). If using VS Code, you can run the task
"Monitor Pico's USB serial output" instead to monitor the standard output.
