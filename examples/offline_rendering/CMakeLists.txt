cmake_minimum_required(VERSION 2.8.12)
project(OfflineRendering)

find_package(VRK REQUIRED)

add_executable(offline_rendering main.cpp)
target_include_directories(offline_rendering PRIVATE ${VRK_INCLUDE_DIRS})
target_link_libraries(offline_rendering PRIVATE vrk)
