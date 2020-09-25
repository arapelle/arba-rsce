cmake_minimum_required(VERSION 3.18)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

include(CMakePrintHelpers)
include(cmtk/CppLibraryProject)

#-----
# PROJECT

check_cmake_binary_dir()
set_build_type_if_undefined()

#-----
# C++ PROJECT

project(rsce
        VERSION 0.1.1
        DESCRIPTION "A C++ library providing tools for managing resources."
        HOMEPAGE_URL "https://github.com/arapelle/rsce"
        LANGUAGES CXX)

message(STATUS "BUILD   : ${CMAKE_BUILD_TYPE}")
message(STATUS "CPPCOMP : ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "PROJECT : ${PROJECT_NAME} ${PROJECT_VERSION}")

#-----
include(CTest)

# Project options
library_build_options(${PROJECT_NAME} STATIC SHARED EXAMPLE TEST)

# Headers:
set(headers
    include/rsce/load_resource_from_file.hpp
    include/rsce/resource_store.hpp
    include/rsce/basic_resource_manager.hpp
    include/rsce/resource_manager.hpp
)

# Sources:
set(sources
    src/resource_store.cpp
)

# Add C++ library
add_cpp_library(${PROJECT_NAME} ${PROJECT_NAME}_BUILD_SHARED_LIB ${PROJECT_NAME}_BUILD_STATIC_LIB
    SHARED ${PROJECT_NAME}
    STATIC ${PROJECT_NAME}-static
    NAMESPACE arba::
    CXX_STANDARD 20
    INCLUDE_DIRECTORIES include
    OUTPUT_VERSION_HEADER "version.hpp"
    HEADERS ${headers}
    SOURCES ${sources}
    BUILT_TARGETS project_targets
    )

# Install C++ library
install_cpp_library_targets(${PROJECT_NAME}
                            TARGETS ${project_targets}
                            INCLUDE_DIRECTORIES "include/${PROJECT_NAME}"
                            NAMESPACE arba::
                            )

# Link targets:
if(NOT TARGET arba::vlfs)
     find_package(vlfs 0.1.0 REQUIRED CONFIG)
endif()
 cpp_library_targets_link_libraries(${PROJECT_NAME} PUBLIC arba::vlfs)

# Install package
install_package(${PROJECT_NAME}
                INPUT_PACKAGE_CONFIG_FILE CMake-package-config.cmake.in
                EXPORT_NAMES ${PROJECT_NAME})

if(${PROJECT_NAME}_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(${PROJECT_NAME}_BUILD_TESTS AND BUILD_TESTING)
    add_subdirectory(test)
endif()

#-----
