add_executable(endless endless.cpp)

target_link_libraries(endless
    PRIVATE
        hdb::project_options
        hdb::project_warnings
)

add_executable(terminated terminated.cpp)

target_link_libraries(terminated
    PRIVATE
        hdb::project_options
        hdb::project_warnings
)

add_executable(rwrite rwrite.s)

set_target_properties(rwrite PROPERTIES POSITION_INDEPENDENT_CODE ON)
