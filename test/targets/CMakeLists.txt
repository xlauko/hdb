add_executable(endless endless.cpp)

target_link_libraries(endless
    PRIVATE
        hdb::project_options
        hdb::project_warnings
)
