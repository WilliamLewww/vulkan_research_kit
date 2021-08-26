cmake_minimum_required(VERSION 2.8.12)
project(offline_rendering)

if(Vulkan_INCLUDE_DIRS)
  set(LOCAL_Vulkan_INCLUDE_DIRS ${Vulkan_INCLUDE_DIRS})
elseif(DEFINED ENV{Vulkan_INCLUDE_DIRS})
  set(LOCAL_Vulkan_INCLUDE_DIRS $ENV{Vulkan_INCLUDE_DIRS})
endif()

find_package(Vulkan QUIET)

if(LOCAL_Vulkan_INCLUDE_DIRS)
  set(Vulkan_INCLUDE_DIRS ${LOCAL_Vulkan_INCLUDE_DIRS})
endif()
if(NOT Vulkan_INCLUDE_DIRS)
  message(FATAL_ERROR "Vulkan_INCLUDE_DIRS not set")
endif()

get_filename_component(VRK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ ABSOLUTE)

find_library(VRK_LIBRARY
	NAMES vrk
	HINTS ${VRK_SOURCE_DIR}/build/
)

if(NOT DEFINED ENV{VRK_RESOURCES_PATH})
	message(FATAL_ERROR "VRK_RESOURCES_PATH environment variable not set (Hint: source setup-env.sh)")
endif()

add_executable(offline_rendering main.cpp)
include_directories(offline_rendering ${Vulkan_INCLUDE_DIRS})
include_directories(offline_rendering ${VRK_SOURCE_DIR}/include)
target_link_libraries(offline_rendering ${VRK_LIBRARY})