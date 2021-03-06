[![Ubuntu CI - Vulkan from SDK](https://github.com/WilliamLewww/vulkan_research_kit/actions/workflows/ubuntu-ci-vulkan-from-sdk.yml/badge.svg)](https://github.com/WilliamLewww/vulkan_research_kit/actions/workflows/ubuntu-ci-vulkan-from-sdk.yml)
[![Ubuntu CI - Vulkan from Source](https://github.com/WilliamLewww/vulkan_research_kit/actions/workflows/ubuntu-ci-vulkan-from-source.yml/badge.svg)](https://github.com/WilliamLewww/vulkan_research_kit/actions/workflows/ubuntu-ci-vulkan-from-source.yml)

# Vulkan Research Kit

The Vulkan Research Kit (VRK) is a thin wrapper of the Vulkan API. The project was made for creating quick prototypes of Vulkan applications/demos.

## Building

The Vulkan Research Kit requires the following components:
- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers)
- [Vulkan-Loader](https://github.com/KhronosGroup/Vulkan-Loader) (Required for libvulkan.so)
- [GLSLang](https://github.com/KhronosGroup/glslang) (Required for compiling GLSL -> SPIR-V)

The following environment variables will be used to build using specified components (the same variables could also be defined when building in cmake):
- `Vulkan_INCLUDE_DIRS`="Vulkan-Headers/build/install/include"
- `Vulkan_LIBRARIES`="Vulkan-Loader/build/install/lib/libvulkan.so"
- `Vulkan_GLSLANG_VALIDATOR_EXECUTABLE`="glslang/build/install/bin/glslangValidator"

### Build Steps

Create a build directory and navigate to the new directory.
```bash
mkdir build
cd build
```
Run cmake with the root of the repository as the argument.
```bash
cmake ..
```

### Building Examples
An included source file can be used to set up the environment variables required for building the examples.
```bash
source tools/setup-vrk-env.sh
```
## Libraries
`vrk`: Provides the base Vulkan implementation. Can be used alone for a headless Vulkan application.

`vrk-wsi`: Provides support to the Window System Integration. Required for presenting images to a surface on a window.

`vrk-ray-tracing`: Provides support to the Vulkan ray tracing extensions.

`vrk-dynamic-rendering`: Provides support to the Vulkan dynamic rendering extension.

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
