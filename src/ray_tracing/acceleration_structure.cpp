#include "vrk/ray_tracing/acceleration_structure.h"

AccelerationStructure::AccelerationStructureBuildSizesInfoParam
AccelerationStructure::getAccelerationStructureBuildSizes(
    VkDevice &deviceHandleRef,
    VkAccelerationStructureBuildTypeKHR accelerationStructureBuildType,
    VkAccelerationStructureTypeKHR accelerationStructureType,
    VkBuildAccelerationStructureFlagsKHR buildAccelerationStructureFlags,
    std::vector<AccelerationStructureGeometryParam>
        accelerationStructureGeometryParamList,
    std::vector<uint32_t> maxPrimitiveCountList) {

  std::vector<VkAccelerationStructureGeometryKHR>
      accelerationStructureGeometryList = {};

  for (AccelerationStructureGeometryParam &accelerationStructureGeometryParam :
       accelerationStructureGeometryParamList) {

    AccelerationStructureGeometryDataParam
        &accelerationStructureGeometryDataParam =
            accelerationStructureGeometryParam
                .accelerationStructureGeometryDataParam;

    VkAccelerationStructureGeometryDataKHR accelerationStructureGeometryData;

    if (accelerationStructureGeometryParam.geometryType ==
        VK_GEOMETRY_TYPE_TRIANGLES_KHR) {

      accelerationStructureGeometryData = {
          .triangles = {
              .sType =
                  VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR,
              .pNext = NULL,
              .vertexFormat =
                  accelerationStructureGeometryDataParam
                      .accelerationStructureGeometryTrianglesDataParam
                      .vertexFormat,
              .vertexData = {.deviceAddress = 0},
              .vertexStride =
                  accelerationStructureGeometryDataParam
                      .accelerationStructureGeometryTrianglesDataParam
                      .vertexStride,
              .maxVertex = accelerationStructureGeometryDataParam
                               .accelerationStructureGeometryTrianglesDataParam
                               .maxVertex,
              .indexType = accelerationStructureGeometryDataParam
                               .accelerationStructureGeometryTrianglesDataParam
                               .indexType,
              .indexData = {.deviceAddress = 0},
              .transformData = {.deviceAddress = 0}}};
    }

    if (accelerationStructureGeometryParam.geometryType ==
        VK_GEOMETRY_TYPE_AABBS_KHR) {

      accelerationStructureGeometryData = {
          .aabbs = {
              .sType =
                  VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR,
              .pNext = NULL,
              .data = {.deviceAddress = 0},
              .stride =
                  accelerationStructureGeometryDataParam
                      .accelerationStructureGeometryAabbsDataParam.stride}};
    }

    if (accelerationStructureGeometryParam.geometryType ==
        VK_GEOMETRY_TYPE_INSTANCES_KHR) {

      accelerationStructureGeometryData = {
          .instances = {
              .sType =
                  VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR,
              .pNext = NULL,
              .arrayOfPointers =
                  accelerationStructureGeometryDataParam
                      .accelerationStructureGeometryInstancesDataParam
                      .arrayOfPointers,
              .data = {.deviceAddress = 0}}};
    }

    VkAccelerationStructureGeometryKHR accelerationStructureGeometry = {
        .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
        .pNext = NULL,
        .geometryType = accelerationStructureGeometryParam.geometryType,
        .geometry = accelerationStructureGeometryData,
        .flags = accelerationStructureGeometryParam.geometryFlags};

    accelerationStructureGeometryList.push_back(accelerationStructureGeometry);
  }

  VkAccelerationStructureBuildGeometryInfoKHR
      accelerationStructureBuildGeometryInfo = {

          .sType =
              VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
          .pNext = NULL,
          .type = accelerationStructureType,
          .flags = buildAccelerationStructureFlags,
          .mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
          .srcAccelerationStructure = VK_NULL_HANDLE,
          .dstAccelerationStructure = VK_NULL_HANDLE,
          .geometryCount = (uint32_t)accelerationStructureGeometryList.size(),
          .pGeometries = accelerationStructureGeometryList.data(),
          .ppGeometries = NULL,
          .scratchData = {.deviceAddress = 0}};

  VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo =
      {

          .sType =
              VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR,
          .pNext = NULL,
          .accelerationStructureSize = 0,
          .updateScratchSize = 0,
          .buildScratchSize = 0};

  LOAD_DEVICE_FUNCTION(deviceHandleRef, vkGetAccelerationStructureBuildSizesKHR,
                       pvkGetAccelerationStructureBuildSizesKHR);

  pvkGetAccelerationStructureBuildSizesKHR(
      deviceHandleRef, accelerationStructureBuildType,
      &accelerationStructureBuildGeometryInfo, maxPrimitiveCountList.data(),
      &accelerationStructureBuildSizesInfo);

  AccelerationStructureBuildSizesInfoParam
      accelerationStructureBuildSizesInfoParam = {

          .accelerationStructureSize =
              accelerationStructureBuildSizesInfo.accelerationStructureSize,
          .updateScratchSize =
              accelerationStructureBuildSizesInfo.updateScratchSize,
          .buildScratchSize =
              accelerationStructureBuildSizesInfo.buildScratchSize};

  return accelerationStructureBuildSizesInfoParam;
}

void AccelerationStructure::buildAccelerationStructures(
    VkDevice &deviceHandleRef, VkCommandBuffer &commandBufferHandleRef,
    std::vector<AccelerationStructureBuildGeometryInfoParam>
        accelerationStructureBuildGeometryInfoParamList,
    std::vector<VkAccelerationStructureBuildRangeInfoKHR>
        accelerationStructureBuildRangeInfoList) {

  std::vector<VkAccelerationStructureBuildGeometryInfoKHR>
      accelerationStructureBuildGeometryInfoList;

  std::vector<std::vector<VkAccelerationStructureGeometryKHR>>
      accelerationStructureGeometryMainList = {};

  for (AccelerationStructureBuildGeometryInfoParam
           &accelerationStructureBuildGeometryInfoParam :
       accelerationStructureBuildGeometryInfoParamList) {

    accelerationStructureGeometryMainList.push_back({});

    for (AccelerationStructureGeometryParam
             &accelerationStructureGeometryParam :
         accelerationStructureBuildGeometryInfoParam
             .accelerationStructureGeometryParamList) {

      AccelerationStructureGeometryDataParam
          &accelerationStructureGeometryDataParam =
              accelerationStructureGeometryParam
                  .accelerationStructureGeometryDataParam;

      VkAccelerationStructureGeometryDataKHR accelerationStructureGeometryData;

      if (accelerationStructureGeometryParam.geometryType ==
          VK_GEOMETRY_TYPE_TRIANGLES_KHR) {

        accelerationStructureGeometryData = {
            .triangles = {
                .sType =
                    VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR,
                .pNext = NULL,
                .vertexFormat =
                    accelerationStructureGeometryDataParam
                        .accelerationStructureGeometryTrianglesDataParam
                        .vertexFormat,
                .vertexData =
                    accelerationStructureGeometryDataParam
                        .accelerationStructureGeometryTrianglesDataParam
                        .vertexData,
                .vertexStride =
                    accelerationStructureGeometryDataParam
                        .accelerationStructureGeometryTrianglesDataParam
                        .vertexStride,
                .maxVertex =
                    accelerationStructureGeometryDataParam
                        .accelerationStructureGeometryTrianglesDataParam
                        .maxVertex,
                .indexType =
                    accelerationStructureGeometryDataParam
                        .accelerationStructureGeometryTrianglesDataParam
                        .indexType,
                .indexData =
                    accelerationStructureGeometryDataParam
                        .accelerationStructureGeometryTrianglesDataParam
                        .indexData,
                .transformData =
                    accelerationStructureGeometryDataParam
                        .accelerationStructureGeometryTrianglesDataParam
                        .transformData}};
      }

      if (accelerationStructureGeometryParam.geometryType ==
          VK_GEOMETRY_TYPE_AABBS_KHR) {

        accelerationStructureGeometryData = {
            .aabbs = {
                .sType =
                    VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR,
                .pNext = NULL,
                .data = accelerationStructureGeometryDataParam
                            .accelerationStructureGeometryAabbsDataParam.data,
                .stride =
                    accelerationStructureGeometryDataParam
                        .accelerationStructureGeometryAabbsDataParam.stride}};
      }

      if (accelerationStructureGeometryParam.geometryType ==
          VK_GEOMETRY_TYPE_INSTANCES_KHR) {

        accelerationStructureGeometryData = {
            .instances = {
                .sType =
                    VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR,
                .pNext = NULL,
                .arrayOfPointers =
                    accelerationStructureGeometryDataParam
                        .accelerationStructureGeometryInstancesDataParam
                        .arrayOfPointers,
                .data =
                    accelerationStructureGeometryDataParam
                        .accelerationStructureGeometryInstancesDataParam.data}};
      }

      VkAccelerationStructureGeometryKHR accelerationStructureGeometry = {
          .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
          .pNext = NULL,
          .geometryType = accelerationStructureGeometryParam.geometryType,
          .geometry = accelerationStructureGeometryData,
          .flags = accelerationStructureGeometryParam.geometryFlags};

      accelerationStructureGeometryMainList
          [accelerationStructureGeometryMainList.size() - 1]
              .push_back(accelerationStructureGeometry);
    }

    VkAccelerationStructureBuildGeometryInfoKHR
        accelerationStructureBuildGeometryInfo = {

            .sType =
                VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
            .pNext = NULL,
            .type = accelerationStructureBuildGeometryInfoParam
                        .accelerationStructureType,
            .flags = accelerationStructureBuildGeometryInfoParam
                         .buildAccelerationStructureFlags,
            .mode = accelerationStructureBuildGeometryInfoParam
                        .buildAccelerationStructureMode,
            .srcAccelerationStructure =
                accelerationStructureBuildGeometryInfoParam
                    .srcAccelerationStructureHandle,
            .dstAccelerationStructure =
                accelerationStructureBuildGeometryInfoParam
                    .dstAccelerationStructureHandleRef,
            .geometryCount =
                (uint32_t)accelerationStructureGeometryMainList
                    [accelerationStructureGeometryMainList.size() - 1]
                        .size(),
            .pGeometries =
                accelerationStructureGeometryMainList
                    [accelerationStructureGeometryMainList.size() - 1]
                        .data(),
            .ppGeometries = NULL,
            .scratchData = accelerationStructureBuildGeometryInfoParam
                               .scratchDataDeviceOrHostAddress};

    accelerationStructureBuildGeometryInfoList.push_back(
        accelerationStructureBuildGeometryInfo);
  }

  const VkAccelerationStructureBuildRangeInfoKHR *
      *accelerationStructureBuildRangeInfoBuffer =
          (const VkAccelerationStructureBuildRangeInfoKHR **)malloc(
              sizeof(const VkAccelerationStructureBuildRangeInfoKHR *) *
              accelerationStructureBuildRangeInfoList.size());

  for (uint32_t x = 0; x < accelerationStructureBuildRangeInfoList.size();
       x++) {

    accelerationStructureBuildRangeInfoBuffer[x] =
        &accelerationStructureBuildRangeInfoList[x];
  }

  LOAD_DEVICE_FUNCTION(deviceHandleRef, vkCmdBuildAccelerationStructuresKHR,
                       pvkCmdBuildAccelerationStructuresKHR);

  pvkCmdBuildAccelerationStructuresKHR(
      commandBufferHandleRef, accelerationStructureBuildGeometryInfoList.size(),
      accelerationStructureBuildGeometryInfoList.data(),
      accelerationStructureBuildRangeInfoBuffer);

  free(accelerationStructureBuildRangeInfoBuffer);
}

AccelerationStructure::AccelerationStructure(
    VkDevice &deviceHandleRef,
    VkAccelerationStructureCreateFlagsKHR accelerationStructureCreateFlags,
    VkBuffer &bufferHandleRef, VkDeviceSize offsetDeviceSize,
    VkDeviceSize bufferDeviceSize,
    VkAccelerationStructureTypeKHR accelerationStructureType,
    VkDeviceAddress deviceAddress)
    : deviceHandleRef(deviceHandleRef) {

  this->accelerationStructureHandle = VK_NULL_HANDLE;

  VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
      .pNext = NULL,
      .createFlags = accelerationStructureCreateFlags,
      .buffer = bufferHandleRef,
      .offset = offsetDeviceSize,
      .size = bufferDeviceSize,
      .type = accelerationStructureType,
      .deviceAddress = deviceAddress};

  LOAD_DEVICE_FUNCTION(deviceHandleRef, vkCreateAccelerationStructureKHR,
                       pvkCreateAccelerationStructureKHR);

  VkResult result = pvkCreateAccelerationStructureKHR(
      deviceHandleRef, &accelerationStructureCreateInfo, NULL,
      &this->accelerationStructureHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateAccelerationStructureKHR");
  }
}

AccelerationStructure::~AccelerationStructure() {
  LOAD_DEVICE_FUNCTION(deviceHandleRef, vkDestroyAccelerationStructureKHR,
                       pvkDestroyAccelerationStructureKHR);

  pvkDestroyAccelerationStructureKHR(this->deviceHandleRef,
                                     this->accelerationStructureHandle, NULL);
}

VkDeviceAddress AccelerationStructure::getAccelerationStructureDeviceAddress() {

  VkAccelerationStructureDeviceAddressInfoKHR
      accelerationStructureDeviceAddressInfo = {

          .sType =
              VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR,
          .pNext = NULL,
          .accelerationStructure = this->accelerationStructureHandle};

  LOAD_DEVICE_FUNCTION(this->deviceHandleRef,
                       vkGetAccelerationStructureDeviceAddressKHR,
                       pvkGetAccelerationStructureDeviceAddressKHR);

  return pvkGetAccelerationStructureDeviceAddressKHR(
      this->deviceHandleRef, &accelerationStructureDeviceAddressInfo);
}

VkAccelerationStructureKHR &
AccelerationStructure::getAccelerationStructureHandleRef() {

  return this->accelerationStructureHandle;
};
