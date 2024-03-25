FROM nvidia/cuda:12.3.2-devel-ubuntu20.04
MAINTAINER Ben Boeckel <ben.boeckel@kitware.com>

COPY install_deps.sh /root/install_deps.sh
RUN sh /root/install_deps.sh

COPY install_fletch.sh /root/install_fletch.sh
COPY configure_fletch.cmake /root/configure_fletch.cmake
RUN sh /root/install_fletch.sh
