cmake_minimum_required(VERSION 2.8.12)
project(xlib_surface)

if(NOT Vulkan_INCLUDE_DIRS)
  if(DEFINED ENV{Vulkan_INCLUDE_DIRS})
    set(Vulkan_INCLUDE_DIRS $ENV{Vulkan_INCLUDE_DIRS})
  elseif(DEFINED ENV{VULKAN_SDK})
    set(Vulkan_INCLUDE_DIRS $ENV{VULKAN_SDK}/include)
  else()
    message(FATAL_ERROR "Vulkan_INCLUDE_DIRS not set")
  endif()
endif()

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
    COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} --target-env vulkan1.2 -o resources/shaders/${SHADER_NAME}.spv ${SHADER}
    DEPENDS ${SHADER}
  )
  target_sources(xlib_surface PRIVATE resources/shaders/${SHADER_NAME}.spv)
endforeach()