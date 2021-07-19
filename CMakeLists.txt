cmake_minimum_required (VERSION 2.8.12)
project(vulkan_research_kit)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(vrk-app src/main.cpp)

file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

file(GLOB SHADERS "src/shaders/*")

add_library(vrk SHARED
  ${SOURCES}
)
include_directories(vrk include)

include_directories(vrk include)
include_directories(vrk ${Vulkan_INCLUDE_DIRS})
include_directories(vrk ${GLFW_INCLUDE_DIRS})

target_link_libraries(vrk ${Vulkan_LIBRARIES})
target_link_libraries(vrk glfw)
target_link_libraries(vrk m)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
foreach(SHADER ${SHADERS})
  get_filename_component(SHADER_NAME ${SHADER} NAME)

  add_custom_command(
    OUTPUT shaders/${SHADER_NAME}.spv
    COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} --target-env=vulkan1.2 -o shaders/${SHADER_NAME}.spv
    DEPENDS ${SHADER}
  )
  target_sources(vrk PRIVATE shaders/${SHADER_NAME}.spv)
endforeach()

add_custom_target(copy_resources
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
)

add_dependencies(vrk copy_resources)