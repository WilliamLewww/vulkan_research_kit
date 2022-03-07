cmake_minimum_required(VERSION 2.8.12)
project(VRK)

find_package(Vulkan REQUIRED)
find_package(X11 QUIET)

file(GLOB SOURCES "src/*.cpp")
add_library(vrk SHARED ${SOURCES})
target_include_directories(vrk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<BUILD_INTERFACE:${Vulkan_INCLUDE_DIRS}>  
    $<INSTALL_INTERFACE:include>)
target_link_libraries(vrk PUBLIC ${Vulkan_LIBRARIES} m)
install(DIRECTORY include/vrk DESTINATION include) 
install(TARGETS vrk DESTINATION lib EXPORT VRKTargets)

if(NOT DISABLE_WSI AND X11_FOUND)
  file(GLOB SOURCES_WSI "src/wsi/*.cpp")
  add_library(vrk-wsi SHARED ${SOURCES_WSI})
  target_include_directories(vrk-wsi PUBLIC ${X11_INCLUDE_DIR})
  target_link_libraries(vrk-wsi PUBLIC vrk ${X11_LIBRARIES})
  target_compile_definitions(vrk-wsi PUBLIC -DX_LIB_SURFACE)
  install(TARGETS vrk-wsi DESTINATION lib EXPORT VRKTargets)
endif()
if(NOT DISABLE_DYNAMIC_RENDERING)
  file(GLOB SOURCES_DYNAMIC_RENDERING "src/dynamic_rendering/*.cpp")
  add_library(vrk-dynamic-rendering SHARED ${SOURCES_DYNAMIC_RENDERING})
  target_link_libraries(vrk-dynamic-rendering PUBLIC vrk)
  install(TARGETS vrk-dynamic-rendering DESTINATION lib EXPORT VRKTargets)
endif()
if(NOT DISABLE_RAY_TRACING)
  file(GLOB SOURCES_RAY_TRACING "src/ray_tracing/*.cpp")
  add_library(vrk-ray-tracing SHARED ${SOURCES_RAY_TRACING})
  target_link_libraries(vrk-ray-tracing PUBLIC vrk)
  install(TARGETS vrk-ray-tracing DESTINATION lib EXPORT VRKTargets)
endif()

set(VRK_INCLUDE_DIRS include ${Vulkan_INCLUDE_DIRS})

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/VRKConfig.cmake.in VRKConfig.cmake
    INSTALL_DESTINATION share/cmake/VRK PATH_VARS VRK_INCLUDE_DIRS)
install(EXPORT VRKTargets FILE VRKTargets.cmake DESTINATION share/cmake/VRK)
install(FILES build/VRKConfig.cmake DESTINATION share/cmake/VRK)
