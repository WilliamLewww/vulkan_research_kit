cmake_minimum_required(VERSION 2.8.12)
project(vulkan_research_kit)

if(NOT Vulkan_INCLUDE_DIRS)
  if(DEFINED ENV{Vulkan_INCLUDE_DIRS})
    set(Vulkan_INCLUDE_DIRS $ENV{Vulkan_INCLUDE_DIRS})
  elseif(DEFINED ENV{VULKAN_SDK})
    set(Vulkan_INCLUDE_DIRS $ENV{VULKAN_SDK}/include)
  else()
    message(FATAL_ERROR "Vulkan_INCLUDE_DIRS not set")
  endif()
endif()

if(NOT Vulkan_LIBRARIES)
  if(DEFINED ENV{Vulkan_LIBRARIES})
    set(Vulkan_LIBRARIES $ENV{Vulkan_LIBRARIES})
  elseif(DEFINED ENV{VULKAN_SDK})
    set(Vulkan_LIBRARIES $ENV{VULKAN_SDK}/lib/libvulkan.so)
  else()
    message(FATAL_ERROR "Vulkan_LIBRARIES not set")
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

find_package(GTest QUIET)
find_package(X11 QUIET)

file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

file(GLOB SHADERS "src/shaders/*")

add_library(vrk SHARED
  ${SOURCES}
)

include_directories(vrk include)
include_directories(vrk ${Vulkan_INCLUDE_DIRS})
target_link_libraries(vrk ${Vulkan_LIBRARIES})
target_link_libraries(vrk m)

if(NOT DISABLE_WSI AND X11_FOUND)
  file(GLOB SOURCES_WSI "src/wsi/*.cpp")

  add_library(vrk-wsi SHARED
    ${SOURCES_WSI}
  )

  target_link_libraries(vrk-wsi vrk)

  include_directories(vrk-wsi ${X11_INCLUDE_DIR})
  target_link_libraries(vrk-wsi ${X11_LIBRARIES})
  target_compile_definitions(vrk-wsi PRIVATE -DX_LIB_SURFACE)
endif()

if(NOT DISABLE_DYNAMIC_RENDERING)
  file(GLOB SOURCES_DYNAMIC_RENDERING "src/dynamic_rendering/*.cpp")

  add_library(vrk-dynamic-rendering SHARED
    ${SOURCES_DYNAMIC_RENDERING}
  )

  target_link_libraries(vrk-dynamic-rendering vrk)
endif()

if(NOT DISABLE_RAY_TRACING)
  file(GLOB SOURCES_RAY_TRACING "src/ray_tracing/*.cpp")

  add_library(vrk-ray-tracing SHARED
    ${SOURCES_RAY_TRACING}
  )

  target_link_libraries(vrk-ray-tracing vrk)
endif()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/shaders)
foreach(SHADER ${SHADERS})
  get_filename_component(SHADER_NAME ${SHADER} NAME)

  add_custom_command(
    OUTPUT resources/shaders/${SHADER_NAME}.spv
    COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} --target-env vulkan1.1 -o resources/shaders/${SHADER_NAME}.spv ${SHADER}
    DEPENDS ${SHADER}
  )
  target_sources(vrk PRIVATE resources/shaders/${SHADER_NAME}.spv)
endforeach()

if(GTest_FOUND AND BUILD_TESTING)
  enable_testing()

  file(GLOB TEST_SOURCES "tests/*.cpp")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

  foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(FILE_NAME ${TEST_SOURCE} NAME_WE)
    string(TOUPPER ${FILE_NAME} UPPER_FILE_NAME)
    add_executable(${UPPER_FILE_NAME} ${TEST_SOURCE})
    include_directories(${UPPER_FILE_NAME} include)
    include_directories(${UPPER_FILE_NAME} ${GTEST_INCLUDE_DIRS})
    target_link_libraries(${UPPER_FILE_NAME} ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} vrk)
    add_test(${UPPER_FILE_NAME} tests/${UPPER_FILE_NAME})
  endforeach()

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

add_executable(vrk-app src/main.cpp)
target_link_libraries(vrk-app vrk)
