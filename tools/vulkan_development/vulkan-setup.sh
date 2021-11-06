#!/bin/bash

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

for i in "$@"
do
case $i in
    -v=*|--version=*)
    VERSION="${i#*=}"
    ;;
    -stv=*|--spirv-tools-version=*)
    SPIRV_TOOLS_VERSION="${i#*=}"
    ;;
    -shv=*|--spirv-headers-version=*)
    SPIRV_HEADERS_VERSION="${i#*=}"
    ;;
    -gv=*|--glslang-version=*)
    GLSLANG_VERSION="${i#*=}"
    ;;
    -t=*|--threads=*)
    THREADS="${i#*=}"
    ;;
    --default)
    DEFAULT=YES
    ;;
    *)
    ;;
esac
done

if [ "${VERSION}" == "" ]
then
  VERSION='main'
fi

if [ "${SPIRV_TOOLS_VERSION}" == "" ]
then
  SPIRV_TOOLS_VERSION='master'
fi

if [ "${SPIRV_HEADERS_VERSION}" == "" ]
then
  SPIRV_HEADERS_VERSION='master'
fi

if [ "${GLSLANG_VERSION}" == "" ]
then
  GLSLANG_VERSION='master'
fi

if [ "${THREADS}" == "" ]
then
  THREADS='4'
fi

echo "Vulkan Version: ${VERSION}"
echo "SPIRV Tools Version: ${SPIRV_TOOLS_VERSION}"
echo "SPIRV Headers Version: ${SPIRV_HEADERS_VERSION}"
echo "glslang Version: ${GLSLANG_VERSION}"
echo "Threads: ${THREADS}"

if [ ! -d "Vulkan-Headers" ]
then
  git clone https://github.com/KhronosGroup/Vulkan-Headers

  git --git-dir=Vulkan-Headers/.git --work-tree=Vulkan-Headers checkout ${VERSION}

  mkdir Vulkan-Headers/build
  cmake -HVulkan-Headers -BVulkan-Headers/build \
      -DCMAKE_INSTALL_PREFIX=Vulkan-Headers/build/install
  make install -j${THREADS} -C Vulkan-Headers/build
fi

if [ ! -d "Vulkan-Loader" ]
then
  git clone https://github.com/KhronosGroup/Vulkan-Loader

  git --git-dir=Vulkan-Loader/.git --work-tree=Vulkan-Loader checkout ${VERSION}

  mkdir Vulkan-Loader/build
  cmake -HVulkan-Loader -BVulkan-Loader/build \
      -DCMAKE_INSTALL_PREFIX=Vulkan-Loader/build/install \
      -DVULKAN_HEADERS_INSTALL_DIR=$SCRIPTPATH/Vulkan-Headers/build/install
  make install -j${THREADS} -C Vulkan-Loader/build
fi

if [ ! -d "SPIRV-Headers" ]
then
  git clone https://github.com/KhronosGroup/SPIRV-Headers

  git --git-dir=SPIRV-Headers/.git --work-tree=SPIRV-Headers checkout ${SPIRV_HEADERS_VERSION}

  mkdir SPIRV-Headers/build
  cmake -HSPIRV-Headers -BSPIRV-Headers/build \
      -DCMAKE_INSTALL_PREFIX=SPIRV-Headers/build/install
  make install -j${THREADS} -C SPIRV-Headers/build
fi

if [ ! -d "SPIRV-Tools" ]
then
  git clone https://github.com/KhronosGroup/SPIRV-Tools

  git --git-dir=SPIRV-Tools/.git --work-tree=SPIRV-Tools checkout ${SPIRV_TOOLS_VERSION}
  
  ln -sF $SCRIPTPATH/SPIRV-Headers $SCRIPTPATH/SPIRV-Tools/external

  mkdir SPIRV-Tools/build
  cmake -HSPIRV-Tools -BSPIRV-Tools/build \
      -DCMAKE_INSTALL_PREFIX=SPIRV-Tools/build/install
  make install -j${THREADS} -C SPIRV-Tools/build
fi

if [ ! -d "glslang" ]
then
  git clone https://github.com/KhronosGroup/glslang

  git --git-dir=glslang/.git --work-tree=glslang checkout ${GLSLANG_VERSION}

  ln -sF $SCRIPTPATH/SPIRV-Tools $SCRIPTPATH/glslang/External/spirv-tools

  mkdir glslang/build
  cmake -Hglslang -Bglslang/build \
      -DCMAKE_INSTALL_PREFIX=glslang/build/install
  make install -j${THREADS} -C glslang/build
fi

if [ ! -d "Vulkan-ValidationLayers" ]
then
  git clone https://github.com/KhronosGroup/Vulkan-ValidationLayers

  git --git-dir=Vulkan-ValidationLayers/.git --work-tree=Vulkan-ValidationLayers checkout ${VERSION}

  git clone https://github.com/martinus/robin-hood-hashing.git \
      Vulkan-ValidationLayers/external/robin-hood-hashing

  mkdir Vulkan-ValidationLayers/build
  cmake -HVulkan-ValidationLayers -BVulkan-ValidationLayers/build \
      -DCMAKE_INSTALL_PREFIX=Vulkan-ValidationLayers/build/install \
      -DVULKAN_HEADERS_INSTALL_DIR=$SCRIPTPATH/Vulkan-Headers/build/install \
      -DGLSLANG_INSTALL_DIR=$SCRIPTPATH/glslang/build/install \
      -DSPIRV_HEADERS_INSTALL_DIR=$SCRIPTPATH/SPIRV-Headers/build/install \
      -DSPIRV_TOOLS_INSTALL_DIR=$SCRIPTPATH/SPIRV-Tools/build/install \
      -DROBIN_HOOD_HASHING_INSTALL_DIR=$SCRIPTPATH/Vulkan-ValidationLayers/external/robin-hood-hashing
  make install -j${THREADS} -C Vulkan-ValidationLayers/build
fi

if [ ! -f "setup-env.sh" ]
then
  touch setup-env.sh
  echo 'export Vulkan_INCLUDE_DIRS="'${SCRIPTPATH}'/Vulkan-Headers/build/install/include"' >> setup-env.sh
  echo 'export Vulkan_GLSLANG_VALIDATOR_EXECUTABLE="'${SCRIPTPATH}'/glslang/build/install/bin/glslangValidator"' >> setup-env.sh
  
  echo 'VK_LAYER_PATH="'${SCRIPTPATH}'/Vulkan-ValidationLayers/build/install/share/vulkan/explicit_layer.d"' >> setup-env.sh
  echo 'export VK_LAYER_PATH' >> setup-env.sh
  
  echo 'Vulkan_LIBRARIES="'${SCRIPTPATH}'/Vulkan-Loader/build/install/lib/libvulkan.so"' >> setup-env.sh
  echo 'Vulkan_LIBRARIES="$Vulkan_LIBRARIES;'${SCRIPTPATH}'/Vulkan-ValidationLayers/build/install/lib/libVkLayer_khronos_validation.so"' >> setup-env.sh
  echo 'export Vulkan_LIBRARIES' >> setup-env.sh
fi
