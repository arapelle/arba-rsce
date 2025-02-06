include(cmtk/CppLibraryTests)
include(GoogleTest)

find_package(GTest 1.14 CONFIG REQUIRED)

add_cpp_library_basic_tests(${PROJECT_TARGET_NAME} GTest::gtest_main
    SOURCES
        project_version_tests.cpp
)

add_library(ut_common INTERFACE)
target_compile_definitions(ut_common INTERFACE -DRSC_PATH="${CMAKE_CURRENT_LIST_DIR}/rsc")

add_cpp_library_basic_tests(${PROJECT_TARGET_NAME} GTest::gtest_main
    SOURCES
        resource_store_tests.cpp
        basic_resource_manager_tests.cpp
        basic_resource_manager_mngr_tests.cpp
        resource_manager_tests.cpp
        resource_manager_mngr_tests.cpp
    DEPENDENCIES
        ut_common
)
