include(CTest)

find_package(doctest CONFIG REQUIRED)

add_executable(hdb-test
    main.cpp
    process.cpp
)

target_link_libraries(hdb-test
    PRIVATE
        doctest::doctest
        hdb::libhdb
        hdb::project_options
        hdb::project_warnings
)

add_test(NAME hdb-test COMMAND hdb-test)

set_tests_properties(hdb-test PROPERTIES
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

