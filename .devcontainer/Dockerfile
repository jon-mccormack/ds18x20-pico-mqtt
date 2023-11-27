# Start with a base image containing the necessary tools
FROM mcr.microsoft.com/devcontainers/cpp:1-debian-11

# Install the necessary packages
RUN apt-get update && apt-get install -y \
  cmake \
  gcc-arm-none-eabi \
  libnewlib-arm-none-eabi \
  libstdc++-arm-none-eabi-newlib \
  git \
  python3 \
  g++ \
  minicom \
  && rm -rf /var/lib/apt/lists/*

# Clone the Raspberry Pi Pico SDK repository
# RUN git clone https://github.com/raspberrypi/pico-sdk.git
# RUN cd pico-sdk && git submodule update --init

# Set the PICO_SDK_PATH environment variable
# ENV PICO_SDK_PATH=/pico-sdk
