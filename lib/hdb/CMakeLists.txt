
#
# libhdb module
#
add_library(libhdb)

target_sources(libhdb
    PUBLIC
        FILE_SET CXX_MODULES
        FILES
            error.cpp
            libhdb.cpp
            pipe.cpp
            process.cpp
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
        hdb::readline
)
