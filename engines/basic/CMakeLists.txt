cmake_minimum_required(VERSION 2.8.12)
project(basic-engine)

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

file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(basic-engine SHARED
  ${SOURCES}
)

include_directories(basic-engine include)
include_directories(basic-engine ${Vulkan_INCLUDE_DIRS})
include_directories(basic-engine ${X11_INCLUDE_DIR})
include_directories(basic-engine ${VRK_SOURCE_DIR}/include)
target_link_libraries(basic-engine ${VRK_LIBRARY} ${VRK_WSI_LIBRARY} -lX11)

file(GLOB SHADERS "src/shaders/*")
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/shaders)
foreach(SHADER ${SHADERS})
  get_filename_component(SHADER_NAME ${SHADER} NAME)

  add_custom_command(
    OUTPUT resources/shaders/${SHADER_NAME}.spv
    COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} --target-env vulkan1.2 -o resources/shaders/${SHADER_NAME}.spv ${SHADER}
    DEPENDS ${SHADER}
  )
  target_sources(basic-engine PRIVATE resources/shaders/${SHADER_NAME}.spv)
endforeach()

add_executable(basic-engine-app src/main.cpp)
target_link_libraries(basic-engine-app basic-engine)
