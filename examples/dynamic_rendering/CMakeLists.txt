cmake_minimum_required(VERSION 2.8.12)
project(dynamic_rendering)

find_package(Vulkan REQUIRED)

get_filename_component(VRK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ ABSOLUTE)

find_library(VRK_LIBRARY
	NAMES vrk
	HINTS ${VRK_SOURCE_DIR}/build/
)

find_library(VRK_DYNAMIC_RENDERING_LIBRARY
	NAMES vrk-dynamic-rendering
	HINTS ${VRK_SOURCE_DIR}/build/
)

if(NOT DEFINED ENV{VRK_RESOURCES_PATH})
	message(FATAL_ERROR "VRK_RESOURCES_PATH environment variable not set (Hint: source setup-env.sh)")
endif()

add_executable(dynamic_rendering main.cpp)
include_directories(dynamic_rendering ${Vulkan_INCLUDE_DIRS})
include_directories(dynamic_rendering ${VRK_SOURCE_DIR}/include)
target_link_libraries(dynamic_rendering ${VRK_LIBRARY} ${VRK_DYNAMIC_RENDERING_LIBRARY})
