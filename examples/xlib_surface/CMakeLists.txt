cmake_minimum_required(VERSION 2.8.12)
project(xlib_surface)

find_package(Vulkan REQUIRED)
find_package(X11 REQUIRED)

get_filename_component(VRK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ ABSOLUTE)

find_library(VRK_LIBRARY
	NAMES vrk
	HINTS ${VRK_SOURCE_DIR}/build/
)

find_library(VRK_WSI_LIBRARY
	NAMES vrk-wsi
	HINTS ${VRK_SOURCE_DIR}/build/
)

if(NOT DEFINED ENV{VRK_RESOURCES_PATH})
	message(FATAL_ERROR "VRK_RESOURCES_PATH environment variable not set (Hint: source setup-env.sh)")
endif()

add_executable(xlib_surface main.cpp)
include_directories(xlib_surface ${Vulkan_INCLUDE_DIRS})
include_directories(xlib_surface ${X11_INCLUDE_DIR})
include_directories(xlib_surface ${VRK_SOURCE_DIR}/include)
target_link_libraries(xlib_surface ${VRK_LIBRARY} ${VRK_WSI_LIBRARY} -lX11)

file(GLOB SHADERS "shaders/*")
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/shaders)
foreach(SHADER ${SHADERS})
  get_filename_component(SHADER_NAME ${SHADER} NAME)

  add_custom_command(
    OUTPUT resources/shaders/${SHADER_NAME}.spv
    COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} --target-env=vulkan1.2 -o resources/shaders/${SHADER_NAME}.spv
    DEPENDS ${SHADER}
  )
  target_sources(xlib_surface PRIVATE resources/shaders/${SHADER_NAME}.spv)
endforeach()