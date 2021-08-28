#include "vrk/ray_tracing/acceleration_structure.h"

AccelerationStructure::AccelerationStructureBuildSizesInfoParam
    AccelerationStructure::getAccelerationStructureBuildSizes(
    VkDevice& deviceHandleRef,
    VkAccelerationStructureBuildTypeKHR accelerationStructureBuildType,
    VkAccelerationStructureTypeKHR accelerationStructureType,
    VkBuildAccelerationStructureFlagsKHR buildAccelerationStructureFlags,
    std::vector<AccelerationStructureGeometryParam>
        accelerationStructureGeometryParamList,
    std::vector<uint32_t> maxPrimitiveCountList) {

  std::vector<VkAccelerationStructureGeometryKHR>
      accelerationStructureGeometryList = {};

  for (AccelerationStructureGeometryParam& accelerationStructureGeometryParam :
      accelerationStructureGeometryParamList) {

    AccelerationStructureGeometryDataParam&
        accelerationStructureGeometryDataParam =
        accelerationStructureGeometryParam.
            accelerationStructureGeometryDataParam;

    VkAccelerationStructureGeometryDataKHR accelerationStructureGeometryData;

    if (accelerationStructureGeometryParam.geometryType ==
        VK_GEOMETRY_TYPE_TRIANGLES_KHR) {

      accelerationStructureGeometryData = {
        .triangles = {
          .sType =
    VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR,
          .pNext = NULL,
          .vertexFormat = accelerationStructureGeometryDataParam.
              accelerationStructureGeometryTrianglesDataParam.vertexFormat,
          .vertexData = {
            .deviceAddress = 0
          },
          .vertexStride = accelerationStructureGeometryDataParam.
              accelerationStructureGeometryTrianglesDataParam.vertexStride,
          .maxVertex = accelerationStructureGeometryDataParam.
              accelerationStructureGeometryTrianglesDataParam.maxVertex,
          .indexType = accelerationStructureGeometryDataParam.
              accelerationStructureGeometryTrianglesDataParam.indexType,
          .indexData = {
            .deviceAddress = 0
          },
          .transformData = {
            .deviceAddress = 0
          }
        }
      };
    }

    if (accelerationStructureGeometryParam.geometryType ==
        VK_GEOMETRY_TYPE_AABBS_KHR) {

      accelerationStructureGeometryData = {
        .aabbs = {
          .sType =
    VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR,
          .pNext = NULL,
          .data = {
            .deviceAddress = 0
          },
          .stride = accelerationStructureGeometryDataParam.
              accelerationStructureGeometryAabbsDataParam.stride
        }
      };
    }

    if (accelerationStructureGeometryParam.geometryType ==
        VK_GEOMETRY_TYPE_INSTANCES_KHR) {

      accelerationStructureGeometryData = {
        .instances = {
          .sType =
    VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR,
          .pNext = NULL,
          .arrayOfPointers = accelerationStructureGeometryDataParam.
              accelerationStructureGeometryInstancesDataParam.arrayOfPointers,
          .data = {
            .deviceAddress = 0
          }
        }
      };
    }

    VkAccelerationStructureGeometryKHR accelerationStructureGeometry = {
      .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
      .pNext = NULL,
      .geometryType = accelerationStructureGeometryParam.geometryType,
      .geometry = accelerationStructureGeometryData,
      .flags = accelerationStructureGeometryParam.geometryFlags
    };

    accelerationStructureGeometryList.push_back(accelerationStructureGeometry);
  }

  VkAccelerationStructureBuildGeometryInfoKHR
      accelerationStructureBuildGeometryInfo = {

    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
    .pNext = NULL,
    .type = accelerationStructureType,
    .flags = buildAccelerationStructureFlags,
    .mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
    .srcAccelerationStructure = VK_NULL_HANDLE,
    .dstAccelerationStructure = VK_NULL_HANDLE,
    .geometryCount = (uint32_t)accelerationStructureGeometryList.size(),
    .pGeometries = accelerationStructureGeometryList.data(),
    .ppGeometries = NULL,
    .scratchData = {
      .deviceAddress = 0
    }
  };

  VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo
      = {

    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR,
    .pNext = NULL,
    .accelerationStructureSize = 0,
    .updateScratchSize = 0,
    .buildScratchSize = 0
  };

  LOAD_DEVICE_FUNCTION(deviceHandleRef,
      vkGetAccelerationStructureBuildSizesKHR,
      pvkGetAccelerationStructureBuildSizesKHR);

  pvkGetAccelerationStructureBuildSizesKHR(deviceHandleRef,
      accelerationStructureBuildType,
      &accelerationStructureBuildGeometryInfo,
      maxPrimitiveCountList.data(),
      &accelerationStructureBuildSizesInfo);

  AccelerationStructureBuildSizesInfoParam
      accelerationStructureBuildSizesInfoParam = {

    .accelerationStructureSize =
        accelerationStructureBuildSizesInfo.accelerationStructureSize,
    .updateScratchSize = accelerationStructureBuildSizesInfo.updateScratchSize,
    .buildScratchSize = accelerationStructureBuildSizesInfo.buildScratchSize
  };

  return accelerationStructureBuildSizesInfoParam;
}

AccelerationStructure::AccelerationStructure(VkDevice& deviceHandleRef,
    VkAccelerationStructureCreateFlagsKHR accelerationStructureCreateFlags,
    VkBuffer& bufferHandleRef,
    VkDeviceSize offsetDeviceSize,
    VkDeviceSize bufferDeviceSize,
    VkAccelerationStructureTypeKHR accelerationStructureType,
    VkDeviceAddress deviceAddress) :
    deviceHandleRef(deviceHandleRef) {

  this->accelerationStructureHandle = VK_NULL_HANDLE;

  VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
    .pNext = NULL,
    .createFlags = accelerationStructureCreateFlags,
    .buffer = bufferHandleRef,
    .offset = offsetDeviceSize,
    .size = bufferDeviceSize,
    .type = accelerationStructureType,
    .deviceAddress = deviceAddress
  };

  LOAD_DEVICE_FUNCTION(deviceHandleRef,
      vkCreateAccelerationStructureKHR,
      pvkCreateAccelerationStructureKHR);

  VkResult result = pvkCreateAccelerationStructureKHR(deviceHandleRef,
      &accelerationStructureCreateInfo, NULL,
      &this->accelerationStructureHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateAccelerationStructureKHR");
  }
}

AccelerationStructure::~AccelerationStructure() {
  LOAD_DEVICE_FUNCTION(deviceHandleRef,
      vkDestroyAccelerationStructureKHR,
      pvkDestroyAccelerationStructureKHR);

  pvkDestroyAccelerationStructureKHR(this->deviceHandleRef,
      this->accelerationStructureHandle, NULL);
}