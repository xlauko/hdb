
#
# libhdb module
#
add_library(libhdb)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.cpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.cpp.in
)

file(GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/config.cpp
    INPUT ${CMAKE_CURRENT_BINARY_DIR}/config.cpp.in
)

target_sources(libhdb
    PUBLIC
        FILE_SET CXX_MODULES
        FILES
            bits.cpp
            common.cpp
            error.cpp
            libhdb.cpp
            pipe.cpp
            process.cpp
            registers.cpp

            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>>/config.cpp
    BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>>
)

add_library(hdb::libhdb ALIAS libhdb)

set_target_properties(libhdb
    PROPERTIES
        OUTPUT_NAME "hdb"
)

target_link_libraries(libhdb
    PUBLIC
        hdb::project_options
    PRIVATE
        hdb::project_warnings
)
