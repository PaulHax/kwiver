# Same image currently use by .gitlab/ci/os-linux.yml
ARG BASE_IMAGE=kitware/kwiver:ci-ubuntu20_04-20240327

FROM ${BASE_IMAGE}
ARG ENABLE_CUDA=OFF
# current "main", python examples, 15 May 2024
ARG COMMIT_SHA=323eb2e410732f207cbf1b93e5ef95bf4f741c38
LABEL author Aron Helser <aron.helser@kitware.com>

# Install system dependencies
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
  apt-get upgrade -y && \
  apt-get install -y --no-install-recommends xvfb && \
  apt-get clean && \
  rm -rf /var/lib/apt/lists/*

COPY install_kwiver.sh /root/install_kwiver.sh
RUN sh /root/install_kwiver.sh ${COMMIT_SHA}

# Configure entrypoint
RUN bash -c '\
#!/bin/bash\n\
echo -e "source /opt/kitware/kwiver/setup_KWIVER.sh\n\
\n\
# Set up X virtual framebuffer (Xvfb) to support running VTK headless\n\
Xvfb :1 -screen 0 1024x768x16 -nolisten tcp > xvfb.log &\n\
export DISPLAY=:1.0\n\
\n\
/opt/kitware/kwiver/bin/kwiver \$@" > /entrypoint.sh' && \
    chmod +x /entrypoint.sh

ENTRYPOINT [ "bash", "/entrypoint.sh" ]
CMD [ "help" ]
