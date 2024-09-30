#
# readline module
#

add_library(readline)

find_package(PkgConfig REQUIRED)
pkg_check_modules(readline REQUIRED IMPORTED_TARGET readline)

target_sources(readline
    PUBLIC
        FILE_SET CXX_MODULES
        FILES
            readline.cpp
)

add_library(hdb::readline ALIAS readline)

set_target_properties(readline
    PROPERTIES
        OUTPUT_NAME "readline"
)

target_link_libraries(readline
    PUBLIC
        hdb::project_options
        PkgConfig::readline
)
