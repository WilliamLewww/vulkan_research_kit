#pragma once
#include "basic/material.h"

#include "vrk/graphics_pipeline_group.h"

class MaterialRaster : public Material {
public:
  MaterialRaster(std::shared_ptr<Engine> enginePtr, std::string materialName,
                 std::map<ShaderStage, std::string> shaderStageNameMap);

  ~MaterialRaster();

  void render(VkCommandBuffer commandBufferHandle,
              std::shared_ptr<Model> modelPtr) override;

private:
  const MaterialDescriptorCounts RASTER_MATERIAL_DESCRIPTOR_COUNTS = {
      .uniformBufferCount = 0,
      .storageBufferCount = 0,
      .samplerCount = 0,
      .sampledImageCount = 0,
      .accelerationStructureCount = 0};

  std::shared_ptr<DescriptorSetLayout> rasterDescriptorSetLayoutPtr;

  std::unique_ptr<GraphicsPipelineGroup> graphicsPipelineGroupPtr;
};
