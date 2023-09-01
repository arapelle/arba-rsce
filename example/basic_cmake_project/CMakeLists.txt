cmake_minimum_required(VERSION 3.26)

project(basic_cmake_project)

add_executable(${PROJECT_NAME} main.cpp)
add_executable(${PROJECT_NAME}-static main.cpp)
# Find package arba-rsce:
find_package(arba-rsce 0.3.0 CONFIG REQUIRED)
# Use arba-rsce release shared target:
target_link_libraries(${PROJECT_NAME} PRIVATE arba-rsce)
# Use arba-rsce release static target:
target_link_libraries(${PROJECT_NAME}-static PRIVATE arba-rsce-static)
