cmake_minimum_required(VERSION 2.8.12)
project(ComputePipeline)

find_package(VRK REQUIRED)

add_executable(compute_pipeline main.cpp)
target_include_directories(compute_pipeline PRIVATE ${VRK_INCLUDE_DIRS})
target_link_libraries(compute_pipeline PRIVATE vrk)

file(GLOB SHADERS "shaders/*")
vrk_compile_shaders(compute_pipeline resources/shaders "${SHADERS}")
