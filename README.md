# Vulkan Research Kit

The Vulkan Research Kit is an abstraction of the Vulkan API through libraries and headers. The project was made for creating quick prototypes of Vulkan applications/demos.

## Building

The Vulkan Research Kit requires the following components:
- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers)
- [Vulkan-Loader](https://github.com/KhronosGroup/Vulkan-Loader) (Required for libvulkan.so)
- [GLSLang](https://github.com/KhronosGroup/glslang) (Required for compiling GLSL -> SPIR-V)

The following environment variables may be used to pick each component (the same variables could also be defined when building in cmake)
- Vulkan_INCLUDE_DIRS="Vulkan-Headers/build/install/include"
- Vulkan_LIBRARIES="Vulkan-Loader/build/install/lib/libvulkan.so"
- Vulkan_GLSLANG_VALIDATOR_EXECUTABLE="glslang/build/install/bin/glslangValidator"

### Build Steps

Create a build directory and navigate to the new directory
```bash
mkdir build
cd build
```
Run cmake with the root of the repository as the argument
```bash
cmake ..
```

### Building Examples
An included source file can be used to set up the environment variables required for building the examples
```bash
source setup-env.sh
```
## Libraries
vrk: Provides the base Vulkan implementation. Can be used alone for a headless Vulkan application.

vrk-wsi: Provides support to the Window System Integration. Required for presenting images to a surface on a window.

vrk-ray-tracing: Provides support to the Vulkan ray tracing extensions.

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
