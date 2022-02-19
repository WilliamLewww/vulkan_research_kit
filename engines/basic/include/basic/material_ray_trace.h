#pragma once
#include "basic/material.h"

#include <vrk/ray_tracing/acceleration_structure.h>
#include <vrk/ray_tracing/ray_tracing.h>
#include <vrk/ray_tracing/ray_tracing_pipeline_group.h>

class MaterialRayTrace : public Material {
public:
  MaterialRayTrace(std::shared_ptr<Engine> enginePtr, std::string materialName,
                   std::map<ShaderStage, std::string> shaderStageNameMap);

  ~MaterialRayTrace();

  void render(VkCommandBuffer commandBufferHandle,
              std::shared_ptr<Model> modelPtr) override;

  void createBottomLevelAccelerationStructure(std::shared_ptr<Model> modelPtr);

  void createTopLevelAccelerationStructure();

  bool getTopLevelAccelerationStructureExists();

private:
  const MaterialDescriptorCounts RAY_TRACE_MATERIAL_DESCRIPTOR_COUNTS = {
      .uniformBufferCount = 0,
      .storageBufferCount = 0,
      .samplerCount = 0,
      .sampledImageCount = 0,
      .accelerationStructureCount = 1};

  std::shared_ptr<DescriptorSetLayout> rayTraceDescriptorSetLayoutPtr;

  std::unique_ptr<RayTracingPipelineGroup> rayTracingPipelineGroupPtr;

  std::unique_ptr<Buffer> shaderBindingTableBufferPtr;

  VkStridedDeviceAddressRegionKHR rayGenStridedDeviceAddressRegion;

  VkStridedDeviceAddressRegionKHR rayMissStridedDeviceAddressRegion;

  VkStridedDeviceAddressRegionKHR rayHitStridedDeviceAddressRegion;

  VkStridedDeviceAddressRegionKHR callableStridedDeviceAddressRegion;

  std::vector<std::unique_ptr<AccelerationStructure>>
      bottomLevelAccelerationStructurePtrList;

  std::vector<std::shared_ptr<Buffer>>
      bottomLevelAccelerationStructureBufferPtrList;

  std::unique_ptr<AccelerationStructure> topLevelAccelerationStructurePtr;

  std::unique_ptr<Buffer> accelerationStructureInstanceBufferPtr;

  std::unique_ptr<Buffer> topLevelAccelerationStructureBufferPtr;
};
