find_or_fetch_GTest()

add_cpp_library_basic_tests(GTest::gtest_main
    SHARED ${PROJECT_NAME}
    STATIC ${PROJECT_NAME}-static
    SOURCES
        project_version_tests.cpp
)

add_cpp_library_basic_tests(GTest::gtest
    SHARED ${PROJECT_NAME}
    STATIC ${PROJECT_NAME}-static
    SOURCES
        resource_store_tests.cpp
        basic_resource_manager_tests.cpp
        basic_resource_manager_tests_mngr.cpp
        resource_manager_tests.cpp
        resource_manager_tests_mngr.cpp
)
