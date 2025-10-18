# vcpkg triplet: x86-linux-clang-libcxx.cmake

# Target architecture and system
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CMAKE_SYSTEM_NAME Linux)

# Linkage - use dynamic 
set(VCPKG_LIBRARY_LINKAGE dynamic)
set(VCPKG_CRT_LINKAGE dynamic)

# Compiler flags
set(VCPKG_CXX_FLAGS "-stdlib=libc++")
set(VCPKG_C_FLAGS "")

set(VCPKG_CXX_FLAGS_RELEASE "-O3 -stdlib=libc++")
set(VCPKG_C_FLAGS_RELEASE "-O3")

set(VCPKG_CXX_FLAGS_DEBUG "-g -stdlib=libc++")
set(VCPKG_C_FLAGS_DEBUG "-g")

# Compiler specification
set(VCPKG_CMAKE_C_COMPILER clang)
set(VCPKG_CMAKE_CXX_COMPILER clang++)

# Enable experimental import std support for C++26
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD TRUE)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API TRUE)
set(VCPKG_CXX_STANDARD 26)
set(VCPKG_CXX_STANDARD_REQUIRED TRUE)
set(VCPKG_LINKER_FLAGS "-stdlib=libc++ -lc++abi -lunwind -fuse-ld=lld")
