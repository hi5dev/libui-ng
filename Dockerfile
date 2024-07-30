# Usage:
# docker build -t libui
# docker run `
#   -v /run/desktop/mnt/host/wslg/.X11-unix:/tmp/.X11-unix `
#   -v /run/desktop/mnt/host/wslg:/mnt/wslg `
#   -v "$((gl).Path)":/home/libui `
#   -e DISPLAY=:0 `
#   -e WAYLAND_DISPLAY=wayland-0 `
#   -e XDG_RUNTIME_DIR=/mnt/wslg/runtime-dir `
#   -e PULSE_SERVER=/mnt/wslg/PulseServer `
#   --rm `
#   -it bash
#
# run `xhost +local:docker` on a Linux host to allow Docker to forward X11 to the host

# base container with an updated aptitude cache, and that's all
FROM debian:latest AS base
RUN apt-get update -qqy

# everything required to launch a GUI-based application from Docker
FROM base AS ui
RUN <<SH
apt-get install -qqy gedit x11-apps
apt-get -qqy autoclean
apt-get -qqy clean
SH

# install the build tools
FROM ui AS build-tools
RUN apt-get install -qqy cmake doxygen g++ gcc-12 gdb ninja-build

# install third-party libraries and other dependencies
FROM build-tools AS build-deps
RUN apt-get install -qqy libatk1.0-dev libgtk-3-dev libgtk2.0-dev libx11-dev
