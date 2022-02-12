#pragma once
#include "basic/material.h"

class MaterialRaster : public Material {
public:
  MaterialRaster(std::shared_ptr<Engine> enginePtr, std::string materialName,
                 std::map<ShaderStage, std::string> shaderStageNameMap);

  ~MaterialRaster();

  void render(VkCommandBuffer commandBufferHandle,
              std::shared_ptr<Model> modelPtr) override;

private:
  const MaterialDescriptorCounts RASTER_MATERIAL_DESCRIPTOR_COUNTS = {
      .uniformBufferCount = 50,
      .storageBufferCount = 32,
      .samplerCount = 1,
      .sampledImageCount = 32};

  std::shared_ptr<DescriptorSetLayout> rasterDescriptorSetLayoutPtr;

  std::unique_ptr<GraphicsPipelineGroup> graphicsPipelineGroupPtr;
};
