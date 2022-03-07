cmake_minimum_required(VERSION 2.8.12)
project(RayQuery)

find_package(VRK REQUIRED)

add_executable(ray_query main.cpp)
target_include_directories(ray_query PRIVATE ${VRK_INCLUDE_DIRS})
target_link_libraries(ray_query PRIVATE vrk vrk-ray-tracing)

file(GLOB SHADERS "shaders/*")
vrk_compile_shaders(ray_query resources/shaders "${SHADERS}")
