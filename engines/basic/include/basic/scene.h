#pragma once
#include "basic/camera.h"
#include "basic/material.h"
#include "basic/model.h"

#include <vrk/fence.h>
#include <vrk/semaphore.h>

#include <map>

class Engine;

class Scene : public std::enable_shared_from_this<Scene> {
public:
  Scene(std::string sceneName, std::shared_ptr<Engine> enginePtr);

  ~Scene();

  std::shared_ptr<Material> createMaterial(std::string materialName,
                                           std::string vertexFileName,
                                           std::string fragmentFileName);

  std::shared_ptr<Model> createModel(std::string modelName,
                                     std::string modelPath,
                                     std::shared_ptr<Material> materialPtr);

  void recordCommandBuffer(uint32_t frameIndex);

  std::vector<std::shared_ptr<Material>> getMaterialPtrList();

private:
  std::string sceneName;

  std::shared_ptr<Engine> enginePtr;

  std::vector<std::shared_ptr<Material>> materialPtrList;

  std::vector<std::shared_ptr<Model>> modelPtrList;

  std::map<uint32_t, std::shared_ptr<Model>> indexModelMap;

  uint32_t getNextAvailableModelIndex();
};
