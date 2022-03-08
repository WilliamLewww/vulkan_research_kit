[![Ubuntu CI - Vulkan from SDK](https://github.com/WilliamLewww/vulkan_research_kit/actions/workflows/ubuntu-ci-vulkan-from-sdk.yml/badge.svg)](https://github.com/WilliamLewww/vulkan_research_kit/actions/workflows/ubuntu-ci-vulkan-from-sdk.yml)
[![Ubuntu CI - Vulkan from Source](https://github.com/WilliamLewww/vulkan_research_kit/actions/workflows/ubuntu-ci-vulkan-from-source.yml/badge.svg)](https://github.com/WilliamLewww/vulkan_research_kit/actions/workflows/ubuntu-ci-vulkan-from-source.yml)

# Vulkan Research Kit

The Vulkan Research Kit (VRK) is a thin wrapper of the Vulkan API. The project was made for creating quick prototypes of Vulkan applications/demos.

## Building
Building the project will generate a CMake configuration file that will be used to provide the VRK CMake package. When building engines and examples, the find_package(VRK) will be used to simplify CMakeLists.txt files.

Required Dependencies:
- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers): Provides Vulkan header files required for compilation.
- [Vulkan-Loader](https://github.com/KhronosGroup/Vulkan-Loader): Provides the Vulkan library used for linking during compilation.
- [Glslang](https://github.com/KhronosGroup/glslang): Provides a binary to compile GLSL shader code into SPIR-V intermediate code.

Optional Dependencies:
- [Vulkan-ValidationLayers](https://github.com/KhronosGroup/Vulkan-ValidationLayers): Provides validation layers and libraries for Vulkan validation.

### Build Steps
The tools/vulkan_development/vulkan-setup.sh script clones and builds all the dependencies for VRK if already not installed.
```
sh tools/vulkan_development/vulkan-setup.sh
source tools/vulkan_development/install/setup.sh
```

When at the top of the cloned repository directory:
```
mkdir build && cd build
cmake .. --build
make install
```

### Building Examples
The tools/build-examples.sh script will merge build all the examples.
```
sh tools/build-examples.sh
```

### Building Engines

## Libraries
`vrk`: Provides the base Vulkan implementation. Can be used alone for a headless Vulkan application.

`vrk-wsi`: Provides support to the Window System Integration. Required for presenting images to a surface on a window.

`vrk-ray-tracing`: Provides support to the Vulkan ray tracing extensions.

`vrk-dynamic-rendering`: Provides support to the Vulkan dynamic rendering extension.

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
