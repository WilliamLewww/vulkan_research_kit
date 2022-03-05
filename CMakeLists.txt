cmake_minimum_required(VERSION 2.8.12)
project(vulkan_research_kit)

find_package(Vulkan REQUIRED)
find_package(GTest QUIET)
find_package(X11 QUIET)

file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

file(GLOB SHADERS "src/shaders/*")

add_library(vrk SHARED
  ${SOURCES}
)

target_include_directories(vrk PUBLIC include ${Vulkan_INCLUDE_DIRS})
target_link_libraries(vrk PUBLIC ${Vulkan_LIBRARIES} m )
install(TARGETS vrk DESTINATION lib)
install(DIRECTORY include/vrk DESTINATION include) 

if(NOT DISABLE_WSI AND X11_FOUND)
  file(GLOB SOURCES_WSI "src/wsi/*.cpp")

  add_library(vrk-wsi SHARED
    ${SOURCES_WSI}
  )

  target_include_directories(vrk-wsi PUBLIC ${X11_INCLUDE_DIR})
  target_link_libraries(vrk-wsi PUBLIC vrk ${X11_LIBRARIES})
  target_compile_definitions(vrk-wsi PRIVATE -DX_LIB_SURFACE)
  install(TARGETS vrk-wsi DESTINATION lib)
endif()

if(NOT DISABLE_DYNAMIC_RENDERING)
  file(GLOB SOURCES_DYNAMIC_RENDERING "src/dynamic_rendering/*.cpp")

  add_library(vrk-dynamic-rendering SHARED
    ${SOURCES_DYNAMIC_RENDERING}
  )

  target_link_libraries(vrk-dynamic-rendering PUBLIC vrk)
  install(TARGETS vrk-dynamic-rendering DESTINATION lib)
endif()

if(NOT DISABLE_RAY_TRACING)
  file(GLOB SOURCES_RAY_TRACING "src/ray_tracing/*.cpp")

  add_library(vrk-ray-tracing SHARED
    ${SOURCES_RAY_TRACING}
  )

  target_link_libraries(vrk-ray-tracing PUBLIC vrk)
  install(TARGETS vrk-ray-tracing DESTINATION lib)
endif()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/shaders)
foreach(SHADER ${SHADERS})
  get_filename_component(SHADER_NAME ${SHADER} NAME)

  add_custom_command(
    OUTPUT resources/shaders/${SHADER_NAME}.spv
    COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} --target-env vulkan1.2 -o resources/shaders/${SHADER_NAME}.spv ${SHADER}
    DEPENDS ${SHADER}
  )
  target_sources(vrk PRIVATE resources/shaders/${SHADER_NAME}.spv)
endforeach()
install(DIRECTORY ${CMAKE_BINARY_DIR}/resources/shaders DESTINATION share/vrk)

add_executable(vrk-app src/main.cpp)
target_link_libraries(vrk-app PUBLIC vrk)
install(TARGETS vrk-app DESTINATION bin)
