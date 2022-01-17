cmake_minimum_required(VERSION 2.8.12)
project(voxels)

get_filename_component(ENGINE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ ABSOLUTE)

find_library(ENGINE_LIBRARY
	NAMES basic-engine
	HINTS ${ENGINE_SOURCE_DIR}/build
)

add_executable(voxels main.cpp)
include_directories(voxels ${ENGINE_SOURCE_DIR}/include)
target_link_libraries(voxels ${ENGINE_LIBRARY})
