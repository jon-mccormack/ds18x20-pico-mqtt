# DS18x20 Pico MQTT

An Mqtt client for the Pico that publishes DS18x20 temperature
readings in a Home Assistant-friendly format.

## Setup

After cloning this repo, ensure that git submodules are also resolved
by running `git submodule update --recursive --init`.

### Pico SDK

TODO(Jon).

### VS Code Dev Container

When building the dev container, your Pico must be connected! This is because
the `/dev/ttyACM0` device is forwarded to the container - so creating the container
will fail if its unreachable.

## Building

Run the following, ensuring to emplace your Wifi's `ssid`
and `password`.

```bash
rm -rf build # Cleanup build dir if it already exists
mkdir build
cd build
cmake -DWIFI_SSID=<ssid> -DWIFI_PASSWORD=<password> ..
cmake --build .
```

You can also build in VS Code using the pre-defined tasks.
You should first define your Wifi configuration in
`.vscode/settings.json` in the following way:

```json
{
    "terminal.integrated.env.linux": {
        "WIFI_SSID": "<ssid>",
        "WIFI_PASSWORD": "<password>"
    }
}
```

Then, run the "Configure CMake" task, followed by the "Build"
task which should create the binaries. 

TODO(Jon): would be nice
to have "Configure CMake" as a pre-build command where the task
would monitor the CMakeLists for changes, and re-run that task
before building if necessary. Then we'd only need to run "build"
or ctrl+shift+b which would be really nice.

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
