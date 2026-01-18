
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
            clap.cpp
            common.cpp
            enums.cpp
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

# Enable C++ reflection support (GCC) only for files that need it
# Note: This is a workaround for GCC bug 122785 where -freflection conflicts
# with C++ modules when standard library types are used in module interfaces.
# Files that import reflection-enabled modules must also have reflection enabled.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # enums.cpp uses template for with std::meta::enumerators_of and ^^enum_t
  set_source_files_properties(enums.cpp PROPERTIES COMPILE_FLAGS "-freflection")
  # Note: libhdb.cpp does NOT import :enums to avoid needing -freflection
  # which would conflict with import std; in other modules (GCC bug 122785)
endif()
