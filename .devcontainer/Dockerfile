FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04

# First, Update the apt's source list and include the sources of the packages.
RUN grep deb /etc/apt/sources.list | \
    sed 's/^deb/deb-src /g' >> /etc/apt/sources.list

# Install compiler, python and build tools for P2996 clang
RUN apt-get update && \
    apt-get install -y --no-install-recommends ca-certificates gnupg \
           build-essential cmake make python3 zlib1g wget subversion unzip ninja-build git && \
    rm -rf /var/lib/apt/lists/*

# Clone and build P2996 clang
RUN git clone --depth=1 --branch p2996  https://github.com/bloomberg/clang-p2996.git /tmp/clang-source
RUN cmake -S /tmp/clang-source/llvm -B /tmp/clang-source/build-llvm -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_ENABLE_ASSERTIONS=ON \
    -DLLVM_UNREACHABLE_OPTIMIZE=ON \
    -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind" \
    -DCLANG_DEFAULT_CXX_STDLIB=libc++ \
    -DLLVM_ENABLE_PROJECTS="clang" \
    -DLIBCXX_ENABLE_MODULES=ON \
    -DLIBCXX_ENABLE_EXPERIMENTAL_LIBRARY=ON \
    -DLIBCXX_CXX_STANDARD=26 \
    -DLIBCXX_CXX_FLAGS="-freflection-latest -fmodules" \
    -DLIBCXXABI_CXX_FLAGS="-freflection-latest" \
    -G Ninja
RUN cmake --build /tmp/clang-source/build-llvm -j16
RUN cmake --install /tmp/clang-source/build-llvm --prefix /usr/local/

#------------------------------------------------------------------------------
# Arguments
#------------------------------------------------------------------------------
ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="4.1.1"

#------------------------------------------------------------------------------
# Core build dependencies and tools
#------------------------------------------------------------------------------
RUN export DEBIAN_FRONTEND=noninteractive \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        ninja-build \
        git \
        python3-pip \
        wget \
        curl \
        unzip \
        ca-certificates \
        gnupg \
        software-properties-common \
        lsb-release \
        gdb \
        pkg-config \
        gcc-multilib g++-multilib libc6-dev-i386 \
        lld \
        libc++-dev libc++abi-dev \
    && rm -rf /var/lib/apt/lists/*

#------------------------------------------------------------------------------
# Install P2996 clang from builder stage
#------------------------------------------------------------------------------
RUN P=`/usr/local/bin/clang++ -v 2>&1 | grep Target | cut -d' ' -f2-`; echo /usr/local/lib/$P > /etc/ld.so.conf.d/$P.conf
RUN ldconfig

# Set up alternatives for P2996 clang
RUN update-alternatives --install /usr/bin/clang clang /usr/local/bin/clang 100 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/local/bin/clang++ 100

#------------------------------------------------------------------------------
# Install GCC (latest available in toolchain PPA)
#------------------------------------------------------------------------------
RUN add-apt-repository ppa:ubuntu-toolchain-r/test -y \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        gcc-14 g++-14 \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100 \
    && rm -rf /var/lib/apt/lists/*

#------------------------------------------------------------------------------
# Reinstall latest CMake (if requested)
#------------------------------------------------------------------------------
COPY ./reinstall-cmake.sh /tmp/reinstall-cmake.sh
RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && \
        /tmp/reinstall-cmake.sh "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}"; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

#------------------------------------------------------------------------------
# Dependency managers: vcpkg
#------------------------------------------------------------------------------
RUN su vscode -c "git clone https://github.com/microsoft/vcpkg.git /home/vscode/vcpkg && \
                  /home/vscode/vcpkg/bootstrap-vcpkg.sh"

#------------------------------------------------------------------------------
# Developer utilities
#------------------------------------------------------------------------------
RUN export DEBIAN_FRONTEND=noninteractive \
    && add-apt-repository ppa:git-core/ppa \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        iwyu \
        bear \
        git \
        nodejs npm \
    && rm -rf /var/lib/apt/lists/*

RUN npm install -g @withgraphite/graphite-cli@stable

#------------------------------------------------------------------------------
# Environment setup
#------------------------------------------------------------------------------
ENV PATH="/usr/local/bin:/home/vscode/vcpkg:${PATH}"
ENV CC=clang
ENV CXX=clang++
