add_executable(hdb hdb.cpp)

target_link_libraries(hdb
    PRIVATE
        hdb::project_options
        hdb::project_warnings
        hdb::libhdb
        hdb::readline
)

include(GNUInstallDirs)
install(
    TARGETS hdb
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
