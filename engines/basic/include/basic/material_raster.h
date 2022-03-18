#pragma once
#include "basic/material.h"

#include <vrk/framebuffer.h>
#include <vrk/graphics_pipeline_group.h>
#include <vrk/image.h>
#include <vrk/render_pass.h>

class MaterialRaster : public Material {
public:
  MaterialRaster(std::shared_ptr<Engine> enginePtr, std::string materialName,
                 std::map<ShaderStage, std::string> shaderStageNameMap);

  ~MaterialRaster();

  void render(VkCommandBuffer commandBufferHandle,
              std::shared_ptr<Model> modelPtr) override;

  std::shared_ptr<RenderPass> getRenderPassPtr();

  std::vector<std::shared_ptr<Framebuffer>> getFramebufferPtrList();

private:
  const MaterialDescriptorCounts RASTER_MATERIAL_DESCRIPTOR_COUNTS = {
      .uniformBufferCount = 0,
      .storageBufferCount = 0,
      .samplerCount = 0,
      .sampledImageCount = 0,
      .storageImageCount = 0,
      .accelerationStructureCount = 0};

  std::shared_ptr<RenderPass> renderPassPtr;

  std::vector<std::shared_ptr<Framebuffer>> framebufferPtrList;

  std::shared_ptr<DescriptorSetLayout> rasterDescriptorSetLayoutPtr;

  std::unique_ptr<GraphicsPipelineGroup> graphicsPipelineGroupPtr;
};
