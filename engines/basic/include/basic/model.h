#pragma once
#include "basic/material.h"

#include <vrk/buffer.h>
#include <vrk/command_buffer_group.h>
#include <vrk/device.h>

#include <string>

class Engine;
class Scene;

class Model : public std::enable_shared_from_this<Model> {
public:
  Model(std::shared_ptr<Engine> enginePtr, std::shared_ptr<Scene> scenePtr,
        std::string modelName, std::string modelPath,
        std::shared_ptr<Material> materialPtr);

  ~Model();

  void
  render(std::shared_ptr<CommandBufferGroup::CommandBufferInheritanceInfoParam>
             commandBufferInheritanceInfoParamPtr,
         uint32_t commandBufferIndex);

private:
  std::shared_ptr<Engine> enginePtr;

  std::shared_ptr<Scene> scenePtr;

  std::string modelName;

  std::shared_ptr<Material> materialPtr;

  std::unique_ptr<Buffer> vertexBufferPtr;

  uint32_t totalIndexCount;

  std::unique_ptr<Buffer> indexBufferPtr;
};
