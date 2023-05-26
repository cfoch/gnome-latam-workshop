FROM ubuntu:jammy

ENV DEBIAN_FRONTEND=noninteractive
RUN apt update && \
    apt install \
        cheese \
        gstreamer1.0-tools \
        libgstreamer-opencv1.0-0 \
        libgstreamer-plugins-bad1.0-dev \
        libgstreamer1.0-dev \
        libopencv-dev \
        meson ninja-build make \
	-y
