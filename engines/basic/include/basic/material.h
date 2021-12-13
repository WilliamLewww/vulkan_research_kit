#pragma once
#include "basic/camera.h"

#include "vrk/graphics_pipeline_group.h"
#include "vrk/pipeline_layout.h"
#include <vrk/descriptor_pool.h>
#include <vrk/descriptor_set_group.h>
#include <vrk/descriptor_set_layout.h>
#include <vrk/device.h>
#include <vrk/resource.h>
#include <vrk/shader_module.h>

#include <memory>

class Engine;
class Scene;
class Light;

class Material {
public:
  Material(std::shared_ptr<Engine> enginePtr, std::string materialName,
           std::string vertexFileName, std::string fragmentFileName);

  ~Material();

  void updateCameraDescriptorSet(std::shared_ptr<Camera> cameraPtr);

  void updateSceneDescriptorSet(std::shared_ptr<Scene> scenePtr);

  void updateEmptyLightDescriptors(std::shared_ptr<Buffer> bufferPtr);

  void updateLightDescriptorSet(std::shared_ptr<Light> lightPtr);

private:
  std::shared_ptr<Engine> enginePtr;

  std::string materialName;

  std::unique_ptr<ShaderModule> vertexShaderModulePtr;

  std::unique_ptr<ShaderModule> fragmentShaderModulePtr;

  std::unique_ptr<PipelineLayout> pipelineLayoutPtr;

  std::unique_ptr<GraphicsPipelineGroup> graphicsPipelineGroupPtr;

  std::unique_ptr<DescriptorPool> descriptorPoolPtr;

  std::unique_ptr<DescriptorSetLayout> descriptorSetLayoutPtr;

  std::unique_ptr<DescriptorSetGroup> descriptorSetGroupPtr;

  friend class Model;
};
