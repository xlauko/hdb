FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04

#------------------------------------------------------------------------------
# Arguments
#------------------------------------------------------------------------------
ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="4.2.1"

#------------------------------------------------------------------------------
# Environment
#------------------------------------------------------------------------------
ENV DEBIAN_FRONTEND=noninteractive
ENV PATH="/usr/local/bin:/home/vscode/vcpkg:${PATH}"

#------------------------------------------------------------------------------
# Base tooling & repositories
#------------------------------------------------------------------------------
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        software-properties-common \
        lsb-release \
        ca-certificates \
        gnupg \
    && add-apt-repository ppa:git-core/ppa -y \
    && rm -rf /var/lib/apt/lists/*

#------------------------------------------------------------------------------
# Core development tools
#------------------------------------------------------------------------------
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        make \
        ninja-build \
        git \
        python3 \
        python3-pip \
        wget \
        curl \
        unzip \
        zlib1g \
        gdb \
        pkg-config \
        lld \
        iwyu \
        bear \
        nodejs \
        npm \
    && rm -rf /var/lib/apt/lists/*

#------------------------------------------------------------------------------
# GCC master build dependencies
#------------------------------------------------------------------------------
RUN apt-get update && apt-get install -y --no-install-recommends \
        flex \
        bison \
        libgmp-dev \
        libmpc-dev \
        libmpfr-dev \
        texinfo \
    && rm -rf /var/lib/apt/lists/*

#------------------------------------------------------------------------------
# Build and install GCC master (trunk)
#------------------------------------------------------------------------------
WORKDIR /opt

RUN git clone https://gcc.gnu.org/git/gcc.git gcc-master-src \
    && cd gcc-master-src \
    && ./contrib/download_prerequisites \
    && mkdir /opt/gcc-master-build \
    && cd /opt/gcc-master-build \
    && ../gcc-master-src/configure \
        --prefix=/opt/gcc-master \
        --disable-multilib \
        --disable-bootstrap \
        --enable-checking=release \
        --enable-languages=c,c++ \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/gcc-master-src /opt/gcc-master-build

#------------------------------------------------------------------------------
# Use GCC master by default
#------------------------------------------------------------------------------
ENV PATH="/opt/gcc-master/bin:${PATH}"
ENV CC=/opt/gcc-master/bin/gcc
ENV CXX=/opt/gcc-master/bin/g++

#------------------------------------------------------------------------------
# Reinstall CMake from source (optional)
#------------------------------------------------------------------------------
COPY ./reinstall-cmake.sh /tmp/reinstall-cmake.sh
RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && \
        /tmp/reinstall-cmake.sh "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}"; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

#------------------------------------------------------------------------------
# Install vcpkg
#------------------------------------------------------------------------------
RUN su vscode -c "git clone https://github.com/microsoft/vcpkg.git /home/vscode/vcpkg && \
                  /home/vscode/vcpkg/bootstrap-vcpkg.sh"

#------------------------------------------------------------------------------
# Global npm tools
#------------------------------------------------------------------------------
RUN npm install -g @withgraphite/graphite-cli@stable
