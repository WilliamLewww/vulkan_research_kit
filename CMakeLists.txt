cmake_minimum_required (VERSION 2.8.12)
project(vulkan_research_kit)

find_package(Vulkan REQUIRED)
find_package(glfw3)
find_package(GTest)

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

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/shaders)
foreach(SHADER ${SHADERS})
  get_filename_component(SHADER_NAME ${SHADER} NAME)

  add_custom_command(
    OUTPUT resources/shaders/${SHADER_NAME}.spv
    COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} --target-env=vulkan1.2 -o resources/shaders/${SHADER_NAME}.spv
    DEPENDS ${SHADER}
  )
  target_sources(vrk PRIVATE resources/shaders/${SHADER_NAME}.spv)
endforeach()

add_custom_target(copy_resources
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
)
add_dependencies(vrk copy_resources)

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

if(glfw3_FOUND)
  add_executable(vrk-app src/main.cpp)
  include_directories(vrk-app ${GLFW_INCLUDE_DIRS})
  target_link_libraries(vrk-app glfw)
  target_link_libraries(vrk-app vrk)
endif()