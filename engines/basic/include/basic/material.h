#pragma once
#include "basic/camera.h"

#include <vrk/descriptor_pool.h>
#include <vrk/descriptor_set_group.h>
#include <vrk/descriptor_set_layout.h>
#include <vrk/device.h>
#include <vrk/image.h>
#include <vrk/image_view.h>
#include <vrk/pipeline_layout.h>
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
  enum class ShaderStage {
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    RAYGEN,
    MISS,
    CLOSEST_HIT,
    ANY_HIT,
    COMPUTE
  };

  enum class MaterialType { RASTER, RAY_TRACE, COMPUTE };

  struct MaterialDescriptorCounts {
    uint32_t uniformBufferCount;
    uint32_t storageBufferCount;
    uint32_t samplerCount;
    uint32_t sampledImageCount;
    uint32_t storageImageCount;
    uint32_t accelerationStructureCount;
  };

  Material(std::shared_ptr<Engine> enginePtr, std::string materialName,
           std::map<ShaderStage, std::string> shaderStageNameMap,
           MaterialType materialType);

  ~Material();

  MaterialType getMaterialType();

  void incrementMaterialPropertiesCount(uint32_t count);

  uint32_t getMaterialPropertiesCount();

  std::shared_ptr<Buffer> getMaterialPropertiesBufferPtr();

  void incrementTextureCount(uint32_t count);

  uint32_t getTextureCount();

  std::shared_ptr<Image> getImagePtr(std::string imageName);

  std::shared_ptr<ImageView> getImageViewPtr(std::string imageViewName);

  std::shared_ptr<DescriptorSetGroup> getDescriptorSetGroupPtr();

  void updateCameraDescriptorSet(std::shared_ptr<Camera> cameraPtr);

  void updateSceneDescriptorSet(std::shared_ptr<Scene> scenePtr);

  void updateLightDescriptorSet(std::shared_ptr<Light> lightPtr);

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

  std::map<ShaderStage, std::unique_ptr<ShaderModule>> shaderStageModulePtrMap;

  std::unique_ptr<PipelineLayout> pipelineLayoutPtr;

  std::unique_ptr<DescriptorPool> descriptorPoolPtr;

  std::unique_ptr<DescriptorSetLayout> sharedDescriptorSetLayoutPtr;

  std::unique_ptr<DescriptorSetLayout> descriptorSetLayoutPtr;

  std::shared_ptr<DescriptorSetGroup> descriptorSetGroupPtr;

  uint32_t materialPropertiesCount;

  std::shared_ptr<Buffer> materialPropertiesBufferPtr;

  std::unique_ptr<Sampler> samplerPtr;

  uint32_t textureCount;

  std::map<std::string, std::shared_ptr<Image>> imagePtrMap;

  std::map<std::string, std::shared_ptr<ImageView>> imageViewPtrMap;

private:
  const MaterialDescriptorCounts SHARED_DESCRIPTOR_COUNTS = {
      .uniformBufferCount = 0,
      .storageBufferCount = 0,
      .samplerCount = 0,
      .sampledImageCount = 0,
      .storageImageCount = 8,
      .accelerationStructureCount = 0};

  const MaterialDescriptorCounts MATERIAL_DESCRIPTOR_COUNTS = {
      .uniformBufferCount = 50,
      .storageBufferCount = 32,
      .samplerCount = 1,
      .sampledImageCount = 32,
      .storageImageCount = 0,
      .accelerationStructureCount = 0};
};
