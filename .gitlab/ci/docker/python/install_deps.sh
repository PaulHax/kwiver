#!/bin/sh

set -e

yum install -y --setopt=install_weak_deps=False \
 git-core gcc-c++

# Development tools
yum install -y --setopt=install_weak_deps=False \
    make cmake ninja-build

# System dependencies
yum install -y --setopt=install_weak_deps=False \
    libuuid-devel
