cmake_minimum_required(VERSION 2.8.12)
project(RayTracingPipeline)

find_package(VRK REQUIRED)

add_executable(ray_tracing_pipeline main.cpp)
target_include_directories(ray_tracing_pipeline PRIVATE ${VRK_INCLUDE_DIRS})
target_link_libraries(ray_tracing_pipeline PRIVATE vrk vrk-ray-tracing)

file(GLOB SHADERS "shaders/*")
vrk_compile_shaders(ray_tracing_pipeline resources/shaders "${SHADERS}")
