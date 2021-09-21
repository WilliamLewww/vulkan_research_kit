cmake_minimum_required(VERSION 2.8.12)
project(ray_query)

if(NOT Vulkan_INCLUDE_DIRS)
  if(DEFINED ENV{Vulkan_INCLUDE_DIRS})
    set(Vulkan_INCLUDE_DIRS $ENV{Vulkan_INCLUDE_DIRS})
  elseif(DEFINED ENV{VULKAN_SDK})
    set(Vulkan_INCLUDE_DIRS $ENV{VULKAN_SDK}/include)
  else()
    message(FATAL_ERROR "Vulkan_INCLUDE_DIRS not set")
  endif()
endif()

if(NOT Vulkan_GLSLANG_VALIDATOR_EXECUTABLE)
  if(DEFINED ENV{Vulkan_GLSLANG_VALIDATOR_EXECUTABLE})
    set(Vulkan_GLSLANG_VALIDATOR_EXECUTABLE $ENV{Vulkan_GLSLANG_VALIDATOR_EXECUTABLE})
  elseif(DEFINED ENV{VULKAN_SDK})
    set(Vulkan_GLSLANG_VALIDATOR_EXECUTABLE $ENV{VULKAN_SDK}/bin/glslangValidator)
  else()
    message(FATAL_ERROR "Vulkan_GLSLANG_VALIDATOR_EXECUTABLE not set")
  endif()
endif()

get_filename_component(VRK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ ABSOLUTE)

find_library(VRK_LIBRARY
	NAMES vrk
	HINTS ${VRK_SOURCE_DIR}/build/
)

find_library(VRK_RAY_TRACING_LIBRARY
	NAMES vrk-ray-tracing
	HINTS ${VRK_SOURCE_DIR}/build/
)

if(NOT DEFINED ENV{VRK_RESOURCES_PATH})
	message(FATAL_ERROR "VRK_RESOURCES_PATH environment variable not set (Hint: source setup-env.sh)")
endif()

add_executable(ray_query main.cpp)
include_directories(ray_query ${Vulkan_INCLUDE_DIRS})
include_directories(ray_query ${VRK_SOURCE_DIR}/include)
target_link_libraries(ray_query ${VRK_LIBRARY} ${VRK_RAY_TRACING_LIBRARY})
