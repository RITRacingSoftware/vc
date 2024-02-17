FROM ubuntu:22.04

USER root

# disable prompts during apt-get
ENV DEBIAN_FRONTEND=noninteractive

# Install required tools
RUN apt-get update && \
apt-get install -y \
pkg-config \
python3 \
python3-pip \
python3-dev \
python-is-python3 \
ruby \
gcc-arm-none-eabi \
valgrind \
git \
cmake \
doxygen \
gdb-multiarch \
graphviz \
openocd && \
rm -rf /var/lib/apt/lists/*

WORKDIR /vc

# Install vector blf libs (https://bitbucket.org/tobylorenz/vector_blf)
COPY libs/ecu-sim/libs/vector_blf vector_blf
RUN cd vector_blf && touch LICENSE.GPL-3.0 && mkdir -p build && cd build && cmake .. && make && make install DESTDIR=.. && make install && /usr/sbin/ldconfig

# Install required python modules
COPY requirements.txt .
RUN python -m pip install -r requirements.txt
