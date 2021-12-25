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
  struct Properties {
    alignas(16) float ambient[4];
    alignas(16) float diffuse[4];
    alignas(16) float specular[4];
    alignas(16) float transmittance[4];
    alignas(16) float emission[4];
    alignas(4) float shininess;
    alignas(4) float ior;
    alignas(4) float dissolve;
    alignas(4) int illum;

    alignas(4) int ambientTextureIndex;
    alignas(4) int diffuseTextureIndex;
    alignas(4) int specularTextureIndex;
  };

  Material(std::shared_ptr<Engine> enginePtr, std::string materialName,
           std::string vertexFileName, std::string fragmentFileName);

  ~Material();

  uint32_t getMaterialPropertiesCount();

  void updateCameraDescriptorSet(std::shared_ptr<Camera> cameraPtr);

  void updateSceneDescriptorSet(std::shared_ptr<Scene> scenePtr);

  void updateEmptyLightDescriptors(std::shared_ptr<Buffer> bufferPtr);

  void updateLightDescriptorSet(std::shared_ptr<Light> lightPtr);

  void appendMaterialsPropertiesBuffer(std::vector<Properties> propertiesList);

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

  uint32_t materialPropertiesCount;

  std::unique_ptr<Buffer> materialPropertiesBufferPtr;

  friend class Model;
};
