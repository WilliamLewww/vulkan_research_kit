cmake_minimum_required(VERSION 2.8.12)
project(BasicEngine)

find_package(VRK REQUIRED)

file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
add_library(basic-engine SHARED ${SOURCES})

target_include_directories(basic-engine PUBLIC include ${VRK_INCLUDE_DIRS})
target_link_libraries(basic-engine PUBLIC vrk vrk-wsi vrk-ray-tracing)

file(GLOB SHADERS "shaders/*")
vrk_compile_shaders(basic-engine resources/shaders "${SHADERS}")

add_executable(basic-engine-app src/main.cpp)
target_link_libraries(basic-engine-app basic-engine)

add_custom_target(copy_resources
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources
      ${CMAKE_CURRENT_BINARY_DIR}/resources
)
add_dependencies(basic-engine-app copy_resources)
