#!/bin/sh

set -e

export DEBIAN_FRONTEND=noninteractive
apt-get update

# Install system dependencies
apt-get install -y --no-install-recommends \
    ca-certificates zlib1g-dev curl

# Install Qt system dependencies. Derived from https://wiki.qt.io/Building_Qt_5_from_Git
apt-get install -y libpng-dev libglx-dev freeglut3-dev libfontconfig1-dev \
    '^libxcb.*-dev' libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev libxkbcommon-dev libxkbcommon-x11-dev \
    libxcursor-dev libicu-dev

# Install Git requirements.
apt-get install -y --no-install-recommends \
    git

# Development tools
apt-get install -y --no-install-recommends \
    cmake make gcc g++ pkg-config cmake-curses-gui nano

# Python dependencies
apt-get install -y --no-install-recommends \
    python3 libpython3-dev python3-distutils python3-setuptools python3-pip \
    python3-venv python3-numpy python-is-python3
# Qt 5.12.8 Qtqml requires a "python", provided by python-is-python3

# Remove unnecessary files
apt-get clean
