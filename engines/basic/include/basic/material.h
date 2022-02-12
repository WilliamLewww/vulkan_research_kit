#pragma once
#include "basic/camera.h"

#include "vrk/graphics_pipeline_group.h"
#include "vrk/pipeline_layout.h"
#include <vrk/descriptor_pool.h>
#include <vrk/descriptor_set_group.h>
#include <vrk/descriptor_set_layout.h>
#include <vrk/device.h>
#include <vrk/image_view.h>
#include <vrk/resource.h>
#include <vrk/sampler.h>
#include <vrk/shader_module.h>

#include <map>
#include <memory>

class Engine;
class Scene;
class Light;
class Model;

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

  enum class ShaderStage {
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    RAYGEN,
    MISS,
    CLOSEST_HIT,
    ANY_HIT
  };

  enum class MaterialType { RASTER, RAY_TRACE };

  struct MaterialDescriptorCounts {
    uint32_t uniformBufferCount;
    uint32_t storageBufferCount;
    uint32_t samplerCount;
    uint32_t sampledImageCount;
  };

  Material(std::shared_ptr<Engine> enginePtr, std::string materialName,
           std::map<ShaderStage, std::string> shaderStageNameMap,
           MaterialType materialType);

  ~Material();

  MaterialType getMaterialType();

  uint32_t getMaterialPropertiesCount();

  uint32_t getTextureCount();

  void updateCameraDescriptorSet(std::shared_ptr<Camera> cameraPtr);

  void updateSceneDescriptorSet(std::shared_ptr<Scene> scenePtr);

  void updateLightDescriptorSet(std::shared_ptr<Light> lightPtr);

  void
  appendMaterialPropertiesDescriptors(std::vector<Properties> propertiesList);

  void appendTextureDescriptors(
      std::vector<std::shared_ptr<ImageView>> imageViewPtrList);

  void updateModelDescriptorSet(std::shared_ptr<Model> modelPtr);

  virtual void render(VkCommandBuffer commandBufferHandle,
                      std::shared_ptr<Model> modelPtr) = 0;

protected:
  void initializeDescriptors(
      const MaterialDescriptorCounts materialDescriptorCounts,
      std::shared_ptr<DescriptorSetLayout> descriptorSetLayoutPtr);

  MaterialType materialType;

  std::shared_ptr<Engine> enginePtr;

  std::string materialName;

  std::map<ShaderStage, std::unique_ptr<ShaderModule>> shaderStageModuleMap;

  std::unique_ptr<PipelineLayout> pipelineLayoutPtr;

  std::unique_ptr<DescriptorPool> descriptorPoolPtr;

  std::unique_ptr<DescriptorSetLayout> descriptorSetLayoutPtr;

  std::unique_ptr<DescriptorSetGroup> descriptorSetGroupPtr;

  uint32_t materialPropertiesCount;

  std::unique_ptr<Buffer> materialPropertiesBufferPtr;

  std::unique_ptr<Sampler> samplerPtr;

  uint32_t textureCount;

private:
  const MaterialDescriptorCounts MATERIAL_DESCRIPTOR_COUNTS = {
      .uniformBufferCount = 50,
      .storageBufferCount = 32,
      .samplerCount = 1,
      .sampledImageCount = 32};
};
