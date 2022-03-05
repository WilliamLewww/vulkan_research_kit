cmake_minimum_required(VERSION 2.8.12)
project(basic-engine)

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

add_custom_target(copy_resources
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
)

add_dependencies(basic-engine-app copy_resources)
