# Base image from Microsoft devcontainers
FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="3.30.3"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

# Add LLVM repository for Noble
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get install -y wget lsb-release software-properties-common gnupg \
    && echo "deb http://apt.llvm.org/noble/ llvm-toolchain-noble-19 main" | tee /etc/apt/sources.list.d/llvm.list \
    && echo "deb-src http://apt.llvm.org/noble/ llvm-toolchain-noble-19 main" | tee -a /etc/apt/sources.list.d/llvm.list \
    && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && apt-get update \
    && apt-get install -y clang-19 lldb-19 lld-19 clangd-19 \
    && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-19 100 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-19 100

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>
