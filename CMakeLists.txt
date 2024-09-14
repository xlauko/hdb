cmake_minimum_required(VERSION 3.30)

if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif ()

if (POLICY CMP0116)
  cmake_policy(SET CMP0116 NEW)
endif ()

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
set(CMAKE_CXX_MODULE_STD 1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND VCPKG_FEATURE_FLAGS "versions")

project(hdb
    LANGUAGES C CXX
    VERSION 0.0.1
    DESCRIPTION "Debugger Toy Project"
    HOMEPAGE_URL "https://github.com/xlauko/hdb"
)

include(prevent_in_source_builds)

#
# setup dependencies
#
find_package(PkgConfig REQUIRED)
pkg_check_modules(readline REQUIRED IMPORTED_TARGET readline)

#
# general project options
#
add_library(hdb_project_options INTERFACE)
add_library(hdb::project_options ALIAS hdb_project_options)
target_compile_features(hdb_project_options INTERFACE cxx_std_26)
set_target_properties(hdb_project_options PROPERTIES CXX_EXTENSIONS OFF)

include(sanitizers)
enable_sanitizers(hdb_project_options)

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  option(HDB_ENABLE_BUILD_WITH_TIME_TRACE
    "Enable -ftime-trace to generate time tracing .json files on clang" OFF
  )
  if (HDB_ENABLE_BUILD_WITH_TIME_TRACE)
    add_compile_definitions(hdb_project_options INTERFACE -ftime-trace)
  endif()
endif()

#
# standard compiler warnings
#
add_library(hdb_project_warnings INTERFACE)
add_library(hdb::project_warnings ALIAS hdb_project_warnings)
include(compiler_warnings)
set_project_warnings(hdb_project_warnings)


#
# allow for static analysis options
#
include(static_analyzers)

add_subdirectory(lib)
add_subdirectory(tools)

option(ENABLE_TESTING "Enable Test Builds" ON)

if (ENABLE_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

include(GNUInstallDirs)

install(TARGETS hdb
    EXPORT hdb-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT hdb-targets
    FILE hdb-config.cmake
    NAMESPACE hdb::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hdb
)
