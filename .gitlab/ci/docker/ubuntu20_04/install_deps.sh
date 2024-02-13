#!/bin/sh

set -e

export DEBIAN_FRONTEND=noninteractive
apt-get update

# Install system dependencies
apt-get install -y --no-install-recommends \
    ca-certificates zlib1g-dev curl

# Install Git requirements.
apt-get install -y --no-install-recommends \
    git

# Development tools
apt-get install -y --no-install-recommends \
    cmake make gcc g++ pkg-config

# Python dependencies
apt-get install -y --no-install-recommends \
    python3 libpython3-dev python3-distutils python3-setuptools python3-pip \
    python3-venv

# Remove unnecessary files
apt-get clean
