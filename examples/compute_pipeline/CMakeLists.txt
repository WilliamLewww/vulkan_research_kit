cmake_minimum_required(VERSION 2.8.12)
project(ComputePipeline)

find_package(VRK REQUIRED)

add_executable(compute_pipeline main.cpp)
target_link_libraries(compute_pipeline)

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
