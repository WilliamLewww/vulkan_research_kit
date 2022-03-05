cmake_minimum_required(VERSION 2.8.12)
project(compute_pipeline)

find_package(Vulkan REQUIRED)

get_filename_component(VRK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ ABSOLUTE)

find_library(VRK_LIBRARY
	NAMES vrk
	HINTS ${VRK_SOURCE_DIR}/build/
)

if(NOT DEFINED ENV{VRK_RESOURCES_PATH})
	message(FATAL_ERROR "VRK_RESOURCES_PATH environment variable not set (Hint: source setup-env.sh)")
endif()

add_executable(compute_pipeline main.cpp)
include_directories(compute_pipeline ${Vulkan_INCLUDE_DIRS})
include_directories(compute_pipeline ${VRK_SOURCE_DIR}/include)
target_link_libraries(compute_pipeline ${VRK_LIBRARY})

file(GLOB SHADERS "shaders/*")
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/shaders)
foreach(SHADER ${SHADERS})
  get_filename_component(SHADER_NAME ${SHADER} NAME)

  add_custom_command(
    OUTPUT resources/shaders/${SHADER_NAME}.spv
    COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} --target-env vulkan1.2 -o resources/shaders/${SHADER_NAME}.spv ${SHADER}
    DEPENDS ${SHADER}
  )
  target_sources(compute_pipeline PRIVATE resources/shaders/${SHADER_NAME}.spv)
endforeach()
