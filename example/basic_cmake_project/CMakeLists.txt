cmake_minimum_required(VERSION 3.18)

project(basic_cmake_project)

add_executable(${PROJECT_NAME} main.cpp)
# Find package rsce:
find_package(rsce 0.1.0 CONFIG REQUIRED)
# Use rsce release shared target:
target_link_libraries(${PROJECT_NAME} PRIVATE rsce)
# Use rsce release static target:
#target_link_libraries(${PROJECT_NAME} PRIVATE rsce-static)

