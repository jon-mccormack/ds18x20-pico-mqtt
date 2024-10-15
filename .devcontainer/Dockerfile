FROM mcr.microsoft.com/devcontainers/cpp:1-debian-11

RUN apt-get update && apt-get install -y \
  cmake \
  gcc-arm-none-eabi \
  git \
  python3 \
  g++ \
  minicom \
  python3-pip \
  && rm -rf /var/lib/apt/lists/*

RUN pip3 install cpplint
