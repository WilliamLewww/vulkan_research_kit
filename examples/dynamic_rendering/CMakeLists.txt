cmake_minimum_required(VERSION 2.8.12)
project(DynamicRendering)

find_package(VRK REQUIRED)

add_executable(dynamic_rendering main.cpp)
target_include_directories(dynamic_rendering PRIVATE ${VRK_INCLUDE_DIRS})
target_link_libraries(dynamic_rendering PRIVATE vrk vrk-dynamic-rendering)
