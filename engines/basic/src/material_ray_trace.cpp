#include "basic/material_ray_trace.h"
#include "basic/engine.h"
#include "basic/scene.h"

MaterialRayTrace::MaterialRayTrace(
    std::shared_ptr<Engine> enginePtr, std::string materialName,
    std::map<ShaderStage, std::string> shaderStageNameMap)
    : Material(enginePtr, materialName, shaderStageNameMap,
               Material::MaterialType::RAY_TRACE) {

  std::vector<RayTracingPipelineGroup::PipelineShaderStageCreateInfoParam>
      pipelineShaderStageCreateInfoList;

  std::vector<RayTracingPipelineGroup::RayTracingShaderGroupCreateInfoParam>
      pipelineShaderGroupCreateInfoList;

  uint32_t rayGenOffset = -1;
  uint32_t rayMissOffset = -1;
  uint32_t rayHitOffset = -1;

  for (auto &pair : this->shaderStageModuleMap) {
    RayTracingPipelineGroup::PipelineShaderStageCreateInfoParam shaderStage = {
        .pipelineShaderStageCreateFlags = 0,
        .shaderStageFlagBits = (VkShaderStageFlagBits)0,
        .shaderModuleHandleRef = pair.second->getShaderModuleHandleRef(),
        .entryPointName = "main",
        .specializationInfoPtr = NULL};

    RayTracingPipelineGroup::RayTracingShaderGroupCreateInfoParam shaderGroup =
        {.rayTracingShaderGroupType = (VkRayTracingShaderGroupTypeKHR)0,
         .generalShader = VK_SHADER_UNUSED_KHR,
         .closestHitShader = VK_SHADER_UNUSED_KHR,
         .anyHitShader = VK_SHADER_UNUSED_KHR,
         .intersectionShader = VK_SHADER_UNUSED_KHR,
         .shaderGroupCaptureReplayHandlePtr = NULL};

    if (pair.first == ShaderStage::RAYGEN) {
      shaderStage.shaderStageFlagBits = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
      shaderGroup.rayTracingShaderGroupType =
          VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
      shaderGroup.generalShader = pipelineShaderGroupCreateInfoList.size();

      if (rayGenOffset == -1) {
        rayGenOffset = pipelineShaderGroupCreateInfoList.size();
      }
    } else if (pair.first == ShaderStage::MISS) {
      shaderStage.shaderStageFlagBits = VK_SHADER_STAGE_MISS_BIT_KHR;
      shaderGroup.rayTracingShaderGroupType =
          VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
      shaderGroup.generalShader = pipelineShaderGroupCreateInfoList.size();

      if (rayMissOffset == -1) {
        rayMissOffset = pipelineShaderGroupCreateInfoList.size();
      }
    } else if (pair.first == ShaderStage::CLOSEST_HIT) {
      shaderStage.shaderStageFlagBits = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
      shaderGroup.rayTracingShaderGroupType =
          VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
      shaderGroup.closestHitShader = pipelineShaderGroupCreateInfoList.size();

      if (rayHitOffset == -1) {
        rayHitOffset = pipelineShaderGroupCreateInfoList.size();
      }
    } else if (pair.first == ShaderStage::ANY_HIT) {
      shaderStage.shaderStageFlagBits = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
      shaderGroup.rayTracingShaderGroupType =
          VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
      shaderGroup.anyHitShader = pipelineShaderGroupCreateInfoList.size();

      if (rayHitOffset == -1) {
        rayHitOffset = pipelineShaderGroupCreateInfoList.size();
      }
    }

    pipelineShaderStageCreateInfoList.push_back(shaderStage);
    pipelineShaderGroupCreateInfoList.push_back(shaderGroup);
  }

  this->rayTraceDescriptorSetLayoutPtr =
      std::shared_ptr<DescriptorSetLayout>(new DescriptorSetLayout(
          enginePtr->getDevicePtr()->getDeviceHandleRef(), 0,
          {{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT |
                VK_SHADER_STAGE_FRAGMENT_BIT,
            NULL},
           {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
            VK_SHADER_STAGE_FRAGMENT_BIT, NULL},
           {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 16,
            VK_SHADER_STAGE_FRAGMENT_BIT, NULL},
           {3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 32,
            VK_SHADER_STAGE_FRAGMENT_BIT, NULL},
           {4, VK_DESCRIPTOR_TYPE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT,
            NULL},
           {5, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 32,
            VK_SHADER_STAGE_FRAGMENT_BIT, NULL},
           {6, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 32,
            VK_SHADER_STAGE_VERTEX_BIT, NULL}}));

  this->initializeDescriptors(this->RAY_TRACE_MATERIAL_DESCRIPTOR_COUNTS,
                              this->rayTraceDescriptorSetLayoutPtr);

  this->rayTracingPipelineGroupPtr =
      std::unique_ptr<RayTracingPipelineGroup>(new RayTracingPipelineGroup(
          enginePtr->getDevicePtr()->getDeviceHandleRef(),
          {{0, pipelineShaderStageCreateInfoList,
            pipelineShaderGroupCreateInfoList, 16, NULL, NULL, NULL,
            this->pipelineLayoutPtr->getPipelineLayoutHandleRef(),
            VK_NULL_HANDLE, 0}}));

  VkPhysicalDeviceRayTracingPipelinePropertiesKHR
      physicalDeviceRayTracingPipelineProperties = {
          .sType =
              VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR,
          .pNext = NULL};

  enginePtr->getDevicePtr()->getPhysicalDeviceProperties2(
      *enginePtr->getPhysicalDeviceHandlePtr().get(),
      {&physicalDeviceRayTracingPipelineProperties});

  VkDeviceSize shaderBindingTableSize =
      physicalDeviceRayTracingPipelineProperties.shaderGroupHandleSize *
      pipelineShaderGroupCreateInfoList.size();

  VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
      .pNext = NULL,
      .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
      .deviceMask = 0};

  this->shaderBindingTableBufferPtr = std::unique_ptr<Buffer>(new Buffer(
      enginePtr->getDevicePtr()->getDeviceHandleRef(),
      *enginePtr->getPhysicalDeviceHandlePtr().get(), 0, shaderBindingTableSize,
      VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      VK_SHARING_MODE_EXCLUSIVE, {enginePtr->getQueueFamilyIndex()},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, {&memoryAllocateFlagsInfo}));

  std::shared_ptr<char> shaderHandleBuffer(new char[shaderBindingTableSize]);
  this->rayTracingPipelineGroupPtr->getRayTracingShaderGroupHandles(
      0, 0, pipelineShaderGroupCreateInfoList.size(), shaderBindingTableSize,
      shaderHandleBuffer);

  void *hostShaderBindingTableBuffer;
  shaderBindingTableBufferPtr->mapMemory(&hostShaderBindingTableBuffer, 0,
                                         shaderBindingTableSize);

  for (uint32_t x = 0; x < pipelineShaderGroupCreateInfoList.size(); x++) {
    memcpy((char *)hostShaderBindingTableBuffer +
               x * physicalDeviceRayTracingPipelineProperties
                       .shaderGroupHandleSize,
           shaderHandleBuffer.get() +
               x * physicalDeviceRayTracingPipelineProperties
                       .shaderGroupHandleSize,
           physicalDeviceRayTracingPipelineProperties.shaderGroupHandleSize);
    hostShaderBindingTableBuffer =
        (char *)hostShaderBindingTableBuffer +
        physicalDeviceRayTracingPipelineProperties.shaderGroupBaseAlignment;
  }
  shaderBindingTableBufferPtr->unmapMemory();

  VkDeviceSize progSize =
      physicalDeviceRayTracingPipelineProperties.shaderGroupBaseAlignment;
  VkDeviceSize sbtSize =
      progSize * (VkDeviceSize)pipelineShaderGroupCreateInfoList.size();

  this->rayGenStridedDeviceAddressRegion = {
      .deviceAddress = shaderBindingTableBufferPtr->getBufferDeviceAddress() +
                       rayGenOffset * progSize,
      .stride = sbtSize,
      .size = sbtSize * rayMissOffset};

  this->rayMissStridedDeviceAddressRegion = {
      .deviceAddress = shaderBindingTableBufferPtr->getBufferDeviceAddress() +
                       rayMissOffset * progSize,
      .stride = sbtSize,
      .size = sbtSize * (rayHitOffset - rayMissOffset)};

  this->rayHitStridedDeviceAddressRegion = {
      .deviceAddress = shaderBindingTableBufferPtr->getBufferDeviceAddress() +
                       rayHitOffset * progSize,
      .stride = sbtSize,
      .size = sbtSize * 1};

  this->callableStridedDeviceAddressRegion = {};
}

MaterialRayTrace::~MaterialRayTrace() {}

void MaterialRayTrace::render(VkCommandBuffer commandBufferHandle,
                              std::shared_ptr<Model> modelPtr) {

  this->rayTracingPipelineGroupPtr->bindPipelineCmd(0, commandBufferHandle);

  this->descriptorSetGroupPtr->bindDescriptorSetsCmd(
      commandBufferHandle, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
      this->pipelineLayoutPtr->getPipelineLayoutHandleRef(), 0, {0, 1}, {});

  this->rayTracingPipelineGroupPtr->traceRaysCmd(
      commandBufferHandle,
      std::make_shared<VkStridedDeviceAddressRegionKHR>(
          this->rayGenStridedDeviceAddressRegion),
      std::make_shared<VkStridedDeviceAddressRegionKHR>(
          this->rayMissStridedDeviceAddressRegion),
      std::make_shared<VkStridedDeviceAddressRegionKHR>(
          this->rayHitStridedDeviceAddressRegion),
      std::make_shared<VkStridedDeviceAddressRegionKHR>(
          this->callableStridedDeviceAddressRegion),
      800, 600, 1);
}

void MaterialRayTrace::createBottomLevelAccelerationStructure(
    std::shared_ptr<Model> modelPtr) {

  AccelerationStructure::AccelerationStructureGeometryDataParam
      accelerationStructureGeometryDataParam = {
          .accelerationStructureGeometryTrianglesDataParam = {
              .vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
              .vertexData = {.deviceAddress = modelPtr->getVertexBufferPtr()
                                                  ->getBufferDeviceAddress()},
              .vertexStride = sizeof(Model::Vertex),
              .maxVertex = modelPtr->getVertexCount(),
              .indexType = VK_INDEX_TYPE_UINT32,
              .indexData = {.deviceAddress = modelPtr->getIndexBufferPtr()
                                                 ->getBufferDeviceAddress()},
              .transformData = {.deviceAddress = 0}}};

  AccelerationStructure::AccelerationStructureBuildSizesInfoParam
      accelerationStructureBuildSizesInfoParam =
          AccelerationStructure::getAccelerationStructureBuildSizes(
              enginePtr->getDevicePtr()->getDeviceHandleRef(),
              VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
              VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, 0,
              {{VK_GEOMETRY_TYPE_TRIANGLES_KHR,
                accelerationStructureGeometryDataParam, 0}},
              {modelPtr->getIndexCount() / 3});

  VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
      .pNext = NULL,
      .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
      .deviceMask = 0};

  this->bottomLevelAccelerationStructureBufferPtrList.push_back(
      std::shared_ptr<Buffer>(new Buffer(
          enginePtr->getDevicePtr()->getDeviceHandleRef(),
          *enginePtr->getPhysicalDeviceHandlePtr().get(), 0,
          accelerationStructureBuildSizesInfoParam.accelerationStructureSize,
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
          VK_SHARING_MODE_EXCLUSIVE, {enginePtr->getQueueFamilyIndex()},
          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {&memoryAllocateFlagsInfo})));

  this->bottomLevelAccelerationStructurePtrList.push_back(
      std::unique_ptr<AccelerationStructure>(new AccelerationStructure(
          enginePtr->getDevicePtr()->getDeviceHandleRef(), 0,
          this->bottomLevelAccelerationStructureBufferPtrList
              [this->bottomLevelAccelerationStructureBufferPtrList.size() - 1]
                  ->getBufferHandleRef(),
          0, accelerationStructureBuildSizesInfoParam.accelerationStructureSize,
          VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, 0)));

  std::shared_ptr<Fence> fencePtr = std::shared_ptr<Fence>(
      new Fence(enginePtr->getDevicePtr()->getDeviceHandleRef(),
                (VkFenceCreateFlagBits)0));

  enginePtr->getUtilityCommandBufferGroupPtr()->beginRecording(
      0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  std::shared_ptr<Buffer> accelerationStructureScratchBuffer =
      std::shared_ptr<Buffer>(new Buffer(
          enginePtr->getDevicePtr()->getDeviceHandleRef(),
          *enginePtr->getPhysicalDeviceHandlePtr().get(), 0,
          accelerationStructureBuildSizesInfoParam.buildScratchSize,
          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
              VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
          VK_SHARING_MODE_EXCLUSIVE, {enginePtr->getQueueFamilyIndex()},
          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {&memoryAllocateFlagsInfo}));

  AccelerationStructure::AccelerationStructureBuildGeometryInfoParam
      accelerationStructureBuildGeometryInfoParam = {
          .accelerationStructureType =
              VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
          .buildAccelerationStructureFlags = 0,
          .buildAccelerationStructureMode =
              VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
          .srcAccelerationStructureHandle = VK_NULL_HANDLE,
          .dstAccelerationStructureHandleRef =
              this->bottomLevelAccelerationStructurePtrList
                  [this->bottomLevelAccelerationStructurePtrList.size() - 1]
                      ->getAccelerationStructureHandleRef(),
          .accelerationStructureGeometryParamList =
              {{VK_GEOMETRY_TYPE_TRIANGLES_KHR,
                accelerationStructureGeometryDataParam,
                VK_GEOMETRY_OPAQUE_BIT_KHR}},
          .scratchDataDeviceOrHostAddress = {
              .deviceAddress = accelerationStructureScratchBuffer
                                   ->getBufferDeviceAddress()}};

  VkAccelerationStructureBuildRangeInfoKHR
      accelerationStructureBuildRangeInfoKHR = {
          .primitiveCount = modelPtr->getIndexCount() / 3,
          .primitiveOffset = 0,
          .firstVertex = 0,
          .transformOffset = 0};

  AccelerationStructure::buildAccelerationStructures(
      enginePtr->getDevicePtr()->getDeviceHandleRef(),
      enginePtr->getUtilityCommandBufferGroupPtr()->getCommandBufferHandleRef(
          0),
      {accelerationStructureBuildGeometryInfoParam},
      {accelerationStructureBuildRangeInfoKHR});

  enginePtr->getUtilityCommandBufferGroupPtr()->endRecording(0);

  enginePtr->getUtilityCommandBufferGroupPtr()->submit(
      enginePtr->getDevicePtr()->getQueueHandleRef(
          enginePtr->getQueueFamilyIndex(), 0),
      {{{}, {}, {0}, {}}}, fencePtr->getFenceHandleRef());

  fencePtr->waitForSignal(UINT32_MAX);
  fencePtr->reset();
}

void MaterialRayTrace::createTopLevelAccelerationStructure() {
  VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
      .pNext = NULL,
      .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
      .deviceMask = 0};

  this->accelerationStructureInstanceBufferPtr =
      std::unique_ptr<Buffer>(new Buffer(
          enginePtr->getDevicePtr()->getDeviceHandleRef(),
          *enginePtr->getPhysicalDeviceHandlePtr().get(), 0,
          sizeof(VkAccelerationStructureInstanceKHR) * 32,
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
              VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
          VK_SHARING_MODE_EXCLUSIVE, {enginePtr->getQueueFamilyIndex()},
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, {&memoryAllocateFlagsInfo}));

  void *hostAccelerationStructureInstanceBuffer;
  this->accelerationStructureInstanceBufferPtr->mapMemory(
      &hostAccelerationStructureInstanceBuffer, 0,
      sizeof(VkAccelerationStructureInstanceKHR) * 32);
  for (uint32_t x = 0; x < this->bottomLevelAccelerationStructurePtrList.size();
       x++) {

    VkAccelerationStructureInstanceKHR accelerationStructureInstance = {
        .transform = {.matrix = {{1.0, 0.0, 0.0, 0.0},
                                 {0.0, 1.0, 0.0, 0.0},
                                 {0.0, 0.0, 1.0, 0.0}}},
        .instanceCustomIndex = 0,
        .mask = 0xFF,
        .instanceShaderBindingTableRecordOffset = 0,
        .flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR,
        .accelerationStructureReference =
            this->bottomLevelAccelerationStructurePtrList[x]
                ->getAccelerationStructureDeviceAddress()};

    memcpy(&((VkAccelerationStructureInstanceKHR *)
                 hostAccelerationStructureInstanceBuffer)[x],
           &accelerationStructureInstance,
           sizeof(VkAccelerationStructureInstanceKHR));
  }
  this->accelerationStructureInstanceBufferPtr->unmapMemory();

  AccelerationStructure::AccelerationStructureGeometryDataParam
      topLevelAccelerationStructureGeometryDataParam = {
          .accelerationStructureGeometryInstancesDataParam = {
              .arrayOfPointers = false,
              .data = {.deviceAddress =
                           this->accelerationStructureInstanceBufferPtr
                               ->getBufferDeviceAddress()},
          }};

  AccelerationStructure::AccelerationStructureBuildSizesInfoParam
      topLevelAccelerationStructureBuildSizesInfoParam =
          AccelerationStructure::getAccelerationStructureBuildSizes(
              enginePtr->getDevicePtr()->getDeviceHandleRef(),
              VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
              VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR, 0,
              {{VK_GEOMETRY_TYPE_INSTANCES_KHR,
                topLevelAccelerationStructureGeometryDataParam,
                VK_GEOMETRY_OPAQUE_BIT_KHR}},
              {(uint32_t)this->bottomLevelAccelerationStructurePtrList.size()});

  this->topLevelAccelerationStructureBufferPtr = std::unique_ptr<Buffer>(
      new Buffer(enginePtr->getDevicePtr()->getDeviceHandleRef(),
                 *enginePtr->getPhysicalDeviceHandlePtr().get(), 0,
                 topLevelAccelerationStructureBuildSizesInfoParam
                     .accelerationStructureSize,
                 VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
                 VK_SHARING_MODE_EXCLUSIVE, {enginePtr->getQueueFamilyIndex()},
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {}));

  this->topLevelAccelerationStructurePtr =
      std::unique_ptr<AccelerationStructure>(new AccelerationStructure(
          enginePtr->getDevicePtr()->getDeviceHandleRef(), 0,
          topLevelAccelerationStructureBufferPtr->getBufferHandleRef(), 0,
          topLevelAccelerationStructureBuildSizesInfoParam
              .accelerationStructureSize,
          VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR, 0));

  std::shared_ptr<Fence> fencePtr = std::shared_ptr<Fence>(
      new Fence(enginePtr->getDevicePtr()->getDeviceHandleRef(),
                (VkFenceCreateFlagBits)0));

  enginePtr->getUtilityCommandBufferGroupPtr()->beginRecording(
      0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  std::shared_ptr<Buffer> accelerationStructureScratchBuffer =
      std::shared_ptr<Buffer>(new Buffer(
          enginePtr->getDevicePtr()->getDeviceHandleRef(),
          *enginePtr->getPhysicalDeviceHandlePtr().get(), 0,
          topLevelAccelerationStructureBuildSizesInfoParam.buildScratchSize,
          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
              VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
          VK_SHARING_MODE_EXCLUSIVE, {enginePtr->getQueueFamilyIndex()},
          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {&memoryAllocateFlagsInfo}));

  AccelerationStructure::AccelerationStructureBuildGeometryInfoParam
      accelerationStructureBuildGeometryInfoParam = {
          .accelerationStructureType =
              VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
          .buildAccelerationStructureFlags = 0,
          .buildAccelerationStructureMode =
              VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
          .srcAccelerationStructureHandle = VK_NULL_HANDLE,
          .dstAccelerationStructureHandleRef =
              this->topLevelAccelerationStructurePtr
                  ->getAccelerationStructureHandleRef(),
          .accelerationStructureGeometryParamList =
              {{VK_GEOMETRY_TYPE_INSTANCES_KHR,
                topLevelAccelerationStructureGeometryDataParam, 0}},
          .scratchDataDeviceOrHostAddress = {
              .deviceAddress = accelerationStructureScratchBuffer
                                   ->getBufferDeviceAddress()}};

  VkAccelerationStructureBuildRangeInfoKHR
      accelerationStructureBuildRangeInfoKHR = {
          .primitiveCount =
              (uint32_t)this->bottomLevelAccelerationStructurePtrList.size(),
          .primitiveOffset = 0,
          .firstVertex = 0,
          .transformOffset = 0};

  AccelerationStructure::buildAccelerationStructures(
      enginePtr->getDevicePtr()->getDeviceHandleRef(),
      enginePtr->getUtilityCommandBufferGroupPtr()->getCommandBufferHandleRef(
          0),
      {accelerationStructureBuildGeometryInfoParam},
      {accelerationStructureBuildRangeInfoKHR});

  enginePtr->getUtilityCommandBufferGroupPtr()->endRecording(0);

  enginePtr->getUtilityCommandBufferGroupPtr()->submit(
      enginePtr->getDevicePtr()->getQueueHandleRef(
          enginePtr->getQueueFamilyIndex(), 0),
      {{{}, {}, {0}, {}}}, fencePtr->getFenceHandleRef());

  fencePtr->waitForSignal(UINT32_MAX);
  fencePtr->reset();
}

bool MaterialRayTrace::getTopLevelAccelerationStructureExists() {
  if (!this->topLevelAccelerationStructurePtr) {
    return false;
  }

  return true;
}
