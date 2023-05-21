FROM ubuntu:jammy

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update

# Install build utils
RUN apt install -y meson ninja-build make
# Install OpenCV
RUN apt install -y libopencv-dev
# Install GStreamer
RUN apt install -y libgstreamer-opencv1.0-0 libgstreamer-plugins-bad1.0-dev libgstreamer1.0-dev

