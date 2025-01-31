include(cmtk/CppLibraryTests)
include(GoogleTest)

find_package(GTest 1.14 CONFIG REQUIRED)

add_cpp_library_basic_tests(${PROJECT_TARGET_NAME} GTest::gtest_main
    SOURCES
        project_version_tests.cpp
)

add_cpp_library_basic_tests(${PROJECT_TARGET_NAME} GTest::gtest
    SOURCES
        resource_store_tests.cpp
        basic_resource_manager_tests.cpp
        basic_resource_manager_mngr_tests.cpp
        resource_manager_tests.cpp
        resource_manager_mngr_tests.cpp
)
