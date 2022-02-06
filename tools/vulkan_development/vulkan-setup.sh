#!/bin/bash
set -e

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
    -nvt|--no-vulkan-tools)
    NO_VULKAN_TOOLS=1
    ;;
    -nvvl|--no-vulkan-validation-layers)
    NO_VULKAN_VALIDATION_LAYERS=1
    ;;
    -d|--delete)
    DELETE=1
    ;;
    *)
    ;;
esac
done

if [ "${DELETE}" == "1" ]
then
  echo "Requesting sudo access to delete existing local repositories:"
  sudo rm -r -f setup-vulkan-development-env.sh glslang SPIRV* Vulkan*
  echo "Local repositories deleted!"
  exit
fi

if [ "${VERSION}" == "" ]
then
  VERSION='sdk-1.2.198.0'
fi

if [ "${SPIRV_TOOLS_VERSION}" == "" ]
then
  SPIRV_TOOLS_VERSION='sdk-1.2.198.0'
fi

if [ "${SPIRV_HEADERS_VERSION}" == "" ]
then
  SPIRV_HEADERS_VERSION='sdk-1.2.198.0'
fi

if [ "${GLSLANG_VERSION}" == "" ]
then
  GLSLANG_VERSION='sdk-1.2.198.0'
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
echo ""
echo "Flags:"
if [ "${NO_VULKAN_TOOLS}" == 1 ]
then
  echo "  No Vulkan Tools"
fi
if [ "${NO_VULKAN_VALIDATION_LAYERS}" == 1 ]
then
  echo "  No Vulkan Validation Layers"
fi

if [ ! -d "Vulkan-Headers/build/install" ]
then
  if [ ! -d "Vulkan-Headers" ]
  then
    git clone https://github.com/KhronosGroup/Vulkan-Headers
  fi

  git --git-dir=Vulkan-Headers/.git --work-tree=Vulkan-Headers checkout ${VERSION}

  mkdir -p Vulkan-Headers/build
  cmake -HVulkan-Headers -BVulkan-Headers/build \
      -DCMAKE_INSTALL_PREFIX=Vulkan-Headers/build/install
  make install -j${THREADS} -C Vulkan-Headers/build
fi

if [ ! -d "Vulkan-Loader/build/install" ]
then
  if [ ! -d "Vulkan-Loader" ]
  then
    git clone https://github.com/KhronosGroup/Vulkan-Loader
  fi

  git --git-dir=Vulkan-Loader/.git --work-tree=Vulkan-Loader checkout ${VERSION}

  mkdir -p Vulkan-Loader/build
  cmake -HVulkan-Loader -BVulkan-Loader/build \
      -DCMAKE_INSTALL_PREFIX=Vulkan-Loader/build/install \
      -DVULKAN_HEADERS_INSTALL_DIR=$SCRIPTPATH/Vulkan-Headers/build/install
  make install -j${THREADS} -C Vulkan-Loader/build
fi

if [ ! -d "Vulkan-Tools/build/install" ] && [ "${NO_VULKAN_TOOLS}" != "1" ]
then
  if [ ! -d "Vulkan-Tools" ]
  then
    git clone https://github.com/KhronosGroup/Vulkan-Tools
  fi

  git --git-dir=Vulkan-Tools/.git --work-tree=Vulkan-Tools checkout ${VERSION}

  mkdir -p Vulkan-Tools/build
  cmake -HVulkan-Tools -BVulkan-Tools/build \
      -DCMAKE_INSTALL_PREFIX=Vulkan-Tools/build/install \
      -DVULKAN_HEADERS_INSTALL_DIR=$SCRIPTPATH/Vulkan-Headers/build/install
  make install -j${THREADS} -C Vulkan-Tools/build
fi

if [ ! -d "SPIRV-Headers/build/install" ]
then
  if [ ! -d "SPIRV-Headers" ]
  then
    git clone https://github.com/KhronosGroup/SPIRV-Headers
  fi

  git --git-dir=SPIRV-Headers/.git --work-tree=SPIRV-Headers checkout ${SPIRV_HEADERS_VERSION}

  mkdir -p SPIRV-Headers/build
  cmake -HSPIRV-Headers -BSPIRV-Headers/build \
      -DCMAKE_INSTALL_PREFIX=SPIRV-Headers/build/install
  make install -j${THREADS} -C SPIRV-Headers/build
fi

if [ ! -d "SPIRV-Tools/build/install" ]
then
  if [ ! -d "SPIRV-Tools" ]
  then
    git clone https://github.com/KhronosGroup/SPIRV-Tools
  fi

  git --git-dir=SPIRV-Tools/.git --work-tree=SPIRV-Tools checkout ${SPIRV_TOOLS_VERSION}
  
  ln -f -sF $SCRIPTPATH/SPIRV-Headers $SCRIPTPATH/SPIRV-Tools/external

  mkdir -p SPIRV-Tools/build
  cmake -HSPIRV-Tools -BSPIRV-Tools/build \
      -DCMAKE_INSTALL_PREFIX=SPIRV-Tools/build/install
  make install -j${THREADS} -C SPIRV-Tools/build
fi

if [ ! -d "glslang/build/install" ]
then
  if [ ! -d "glslang" ]
  then
    git clone https://github.com/KhronosGroup/glslang
  fi

  git --git-dir=glslang/.git --work-tree=glslang checkout ${GLSLANG_VERSION}

  ln -sF $SCRIPTPATH/SPIRV-Tools $SCRIPTPATH/glslang/External/spirv-tools

  mkdir -p glslang/build
  cmake -Hglslang -Bglslang/build \
      -DCMAKE_INSTALL_PREFIX=glslang/build/install
  make install -j${THREADS} -C glslang/build
fi

if [ ! -d "Vulkan-ValidationLayers/build/install" ] && [ "${NO_VULKAN_VALIDATION_LAYERS}" != "1" ]
then
  if [ ! -d "Vulkan-ValidationLayers" ]
  then
    git clone https://github.com/KhronosGroup/Vulkan-ValidationLayers
  fi

  git --git-dir=Vulkan-ValidationLayers/.git --work-tree=Vulkan-ValidationLayers checkout ${VERSION}

  mkdir -p Vulkan-ValidationLayers/build
  cmake -HVulkan-ValidationLayers -BVulkan-ValidationLayers/build \
      -DCMAKE_INSTALL_PREFIX=Vulkan-ValidationLayers/build/install \
      -DVULKAN_HEADERS_INSTALL_DIR=$SCRIPTPATH/Vulkan-Headers/build/install \
      -DGLSLANG_INSTALL_DIR=$SCRIPTPATH/glslang/build/install \
      -DSPIRV_HEADERS_INSTALL_DIR=$SCRIPTPATH/SPIRV-Headers/build/install \
      -DSPIRV_TOOLS_INSTALL_DIR=$SCRIPTPATH/SPIRV-Tools/build/install \
      -DUSE_ROBIN_HOOD_HASHING=OFF
  make install -j${THREADS} -C Vulkan-ValidationLayers/build
fi

if [ ! -f "setup-vulkan-development-env.sh" ]
then
  touch setup-vulkan-development-env.sh
  echo 'export Vulkan_INCLUDE_DIRS="'${SCRIPTPATH}'/Vulkan-Headers/build/install/include"' >> setup-vulkan-development-env.sh
  echo 'export Vulkan_GLSLANG_VALIDATOR_EXECUTABLE="'${SCRIPTPATH}'/glslang/build/install/bin/glslangValidator"' >> setup-vulkan-development-env.sh
 
  if [ "${NO_VULKAN_VALIDATION_LAYERS}" != "1" ]
  then
    echo 'VK_LAYER_PATH="'${SCRIPTPATH}'/Vulkan-ValidationLayers/build/install/share/vulkan/explicit_layer.d"' >> setup-vulkan-development-env.sh
  fi

  echo 'export VK_LAYER_PATH' >> setup-vulkan-development-env.sh
  
  echo 'Vulkan_LIBRARIES="'${SCRIPTPATH}'/Vulkan-Loader/build/install/lib/libvulkan.so"' >> setup-vulkan-development-env.sh

  if [ "${NO_VULKAN_VALIDATION_LAYERS}" != "1" ]
  then
    echo 'Vulkan_LIBRARIES="$Vulkan_LIBRARIES;'${SCRIPTPATH}'/Vulkan-ValidationLayers/build/install/lib/libVkLayer_khronos_validation.so"' >> setup-vulkan-development-env.sh
  fi

  echo 'export Vulkan_LIBRARIES' >> setup-vulkan-development-env.sh
fi
