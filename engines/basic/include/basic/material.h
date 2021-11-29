#pragma once

#include "vrk/graphics_pipeline_group.h"
#include "vrk/pipeline_layout.h"
#include <vrk/device.h>
#include <vrk/resource.h>
#include <vrk/shader_module.h>

#include <memory>

class Engine;

class Material {
public:
  Material(std::shared_ptr<Engine> enginePtr, std::string materialName,
           std::string vertexFileName, std::string fragmentFileName);

  ~Material();

private:
  std::shared_ptr<Engine> enginePtr;

  std::string materialName;

  std::unique_ptr<ShaderModule> vertexShaderModulePtr;

  std::unique_ptr<ShaderModule> fragmentShaderModulePtr;

  std::unique_ptr<GraphicsPipelineGroup> graphicsPipelineGroupPtr;

  friend class Model;
};
