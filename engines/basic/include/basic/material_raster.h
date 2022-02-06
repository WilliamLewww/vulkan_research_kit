#pragma once
#include "basic/material.h"

class MaterialRaster : public Material {
public:
  MaterialRaster(std::shared_ptr<Engine> enginePtr, std::string materialName,
                 std::map<ShaderStage, std::string> shaderStageNameMap);

  ~MaterialRaster();

  void bindPipeline(VkCommandBuffer commandBufferHandle) override;

  void render(VkCommandBuffer commandBufferHandle,
              std::shared_ptr<Model> modelPtr) override;

private:
  std::unique_ptr<GraphicsPipelineGroup> graphicsPipelineGroupPtr;
};
