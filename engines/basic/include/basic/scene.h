#pragma once
#include "basic/camera.h"
#include "basic/light.h"
#include "basic/material.h"
#include "basic/material_compute.h"
#include "basic/material_raster.h"
#include "basic/material_ray_trace.h"
#include "basic/model.h"

#include <vrk/fence.h>
#include <vrk/semaphore.h>

#include <map>

class Engine;

class Scene : public std::enable_shared_from_this<Scene> {
public:
  struct SceneShaderStructure {
    alignas(4) uint32_t lightCount;
  };

  Scene(std::string sceneName, std::shared_ptr<Engine> enginePtr);

  ~Scene();

  std::shared_ptr<Material> createMaterial(
      std::string materialName, Material::MaterialType materialType,
      std::map<Material::ShaderStage, std::string> shaderStageNameMap);

  std::shared_ptr<Model> createModel(std::string modelName,
                                     std::string modelPath,
                                     std::shared_ptr<Material> materialPtr);

  std::shared_ptr<Light> createLight(std::string lightName,
                                     Light::LightType lightType);

  void appendToRenderQueue(std::shared_ptr<Model> modelPtr);

  void appendToRenderQueue(std::shared_ptr<Material> materialPtr);

  void recordCommandBuffer(uint32_t frameIndex);

  std::vector<std::shared_ptr<Material>> getMaterialPtrList();

  std::vector<std::shared_ptr<Light>> getLightPtrList();

  std::vector<std::shared_ptr<Model>> getModelPtrList();

  std::shared_ptr<VkDescriptorBufferInfo> getSceneDescriptorBufferInfoPtr();

private:
  enum class RenderQueueEntryType { MODEL, MATERIAL };

  struct RenderQueueEntry {
    RenderQueueEntryType renderQueueEntryType;
    std::shared_ptr<void> entryPtr;
  };

  std::string sceneName;

  std::shared_ptr<Engine> enginePtr;

  std::vector<std::shared_ptr<Material>> materialPtrList;

  std::vector<std::shared_ptr<Model>> modelPtrList;

  std::map<uint32_t, std::shared_ptr<Model>> indexModelMap;

  std::vector<std::shared_ptr<Light>> lightPtrList;

  SceneShaderStructure sceneShaderStructure;

  std::unique_ptr<Buffer> sceneBufferPtr;

  std::shared_ptr<VkDescriptorBufferInfo> sceneDescriptorBufferInfoPtr;

  uint32_t getNextAvailableModelIndex();

  std::shared_ptr<Buffer> lightsBufferPtr;

  std::shared_ptr<Buffer> modelsBufferPtr;

  std::vector<RenderQueueEntry> renderQueueEntryList;
};
