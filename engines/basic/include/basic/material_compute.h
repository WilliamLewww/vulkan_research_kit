#pragma once
#include "basic/material.h"

#include "vrk/compute_pipeline_group.h"

class MaterialCompute : public Material {
public:
  MaterialCompute(std::shared_ptr<Engine> enginePtr, std::string materialName,
                  std::map<ShaderStage, std::string> shaderStageNameMap);

  ~MaterialCompute();

  void render(VkCommandBuffer commandBufferHandle,
              std::shared_ptr<Model> modelPtr) override;

private:
  const MaterialDescriptorCounts COMPUTE_MATERIAL_DESCRIPTOR_COUNTS = {
      .uniformBufferCount = 0,
      .storageBufferCount = 0,
      .samplerCount = 0,
      .sampledImageCount = 0,
      .accelerationStructureCount = 0};

  std::shared_ptr<DescriptorSetLayout> computeDescriptorSetLayoutPtr;

  std::unique_ptr<ComputePipelineGroup> computePipelineGroupPtr;
};
