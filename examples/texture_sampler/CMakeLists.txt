cmake_minimum_required(VERSION 2.8.12)
project(TextureSampler)

find_package(VRK REQUIRED)

add_executable(texture_sampler main.cpp)
target_include_directories(texture_sampler PRIVATE ${VRK_INCLUDE_DIRS})
target_link_libraries(texture_sampler PRIVATE vrk)

file(GLOB SHADERS "shaders/*")
vrk_compile_shaders(texture_sampler resources/shaders "${SHADERS}")

add_custom_target(copy_resources
  COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
)

add_dependencies(texture_sampler copy_resources)
