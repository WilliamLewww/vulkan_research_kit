cmake_minimum_required(VERSION 2.8.12)
project(XlibSurface)

find_package(VRK REQUIRED)

add_executable(xlib_surface main.cpp)
target_include_directories(xlib_surface PRIVATE ${VRK_INCLUDE_DIRS})
target_link_libraries(xlib_surface PRIVATE vrk vrk-wsi)

file(GLOB SHADERS "shaders/*")
vrk_compile_shaders(xlib_surface resources/shaders "${SHADERS}")
