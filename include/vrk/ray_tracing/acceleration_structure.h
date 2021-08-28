#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class AccelerationStructure {
public:
  struct AccelerationStructureBuildSizesInfoParam {
    VkDeviceSize accelerationStructureSize;
    VkDeviceSize updateScratchSize;
    VkDeviceSize buildScratchSize;
  };

  union AccelerationStructureGeometryDataParam {
    struct AccelerationStructureGeometryTrianglesDataParam {
      VkFormat vertexFormat;
      VkDeviceOrHostAddressConstKHR vertexData;
      VkDeviceSize vertexStride;
      uint32_t maxVertex;
      VkIndexType indexType;
      VkDeviceOrHostAddressConstKHR indexData;
      VkDeviceOrHostAddressConstKHR transformData;
    } accelerationStructureGeometryTrianglesDataParam;

    struct VkAccelerationStructureGeometryAabbsDataKHR {
      VkDeviceOrHostAddressConstKHR data;
      VkDeviceSize stride;
    } accelerationStructureGeometryAabbsDataParam;

    struct VkAccelerationStructureGeometryInstancesDataKHR {
      VkBool32 arrayOfPointers;
      VkDeviceOrHostAddressConstKHR data;
    } accelerationStructureGeometryInstancesDataParam;
  };

  struct AccelerationStructureGeometryParam {
    VkGeometryTypeKHR geometryType;
    AccelerationStructureGeometryDataParam
        accelerationStructureGeometryDataParam;
    VkGeometryFlagsKHR geometryFlags;
  };

  static AccelerationStructureBuildSizesInfoParam
      getAccelerationStructureBuildSizes(
      VkDevice& deviceHandleRef,
      VkAccelerationStructureBuildTypeKHR accelerationStructureBuildType,
      VkAccelerationStructureTypeKHR accelerationStructureType,
      VkBuildAccelerationStructureFlagsKHR buildAccelerationStructureFlags,
      std::vector<AccelerationStructureGeometryParam>
          accelerationStructureGeometryParamList,
      std::vector<uint32_t> maxPrimitiveCountList);

  AccelerationStructure(VkDevice& deviceHandleRef,
      VkAccelerationStructureCreateFlagsKHR accelerationStructureCreateFlags,
      VkBuffer& bufferHandleRef,
      VkDeviceSize offsetDeviceSize,
      VkDeviceSize bufferDeviceSize,
      VkAccelerationStructureTypeKHR accelerationStructureType,
      VkDeviceAddress deviceAddress);

  ~AccelerationStructure();
private:
  VkAccelerationStructureKHR accelerationStructureHandle;

  VkDevice& deviceHandleRef;
};