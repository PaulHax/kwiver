FROM ubuntu:20.04
MAINTAINER Ben Boeckel <ben.boeckel@kitware.com>

COPY install_deps.sh /root/install_deps.sh
RUN sh /root/install_deps.sh

COPY install_fletch.sh /root/install_fletch.sh
RUN sh /root/install_fletch.sh
