#pragma once
#include "basic/material.h"

#include <vrk/buffer.h>
#include <vrk/device.h>

#include <string>

class Engine;

class Model {
public:
  Model(std::shared_ptr<Engine> enginePtr, std::string modelName,
        std::string modelPath, std::shared_ptr<Material> materialPtr);

  ~Model();

  void render(uint32_t frameIndex);

private:
  std::shared_ptr<Engine> enginePtr;

  std::string modelName;

  std::shared_ptr<Material> materialPtr;

  std::unique_ptr<Buffer> vertexBufferPtr;

  uint32_t totalIndexCount;

  std::unique_ptr<Buffer> indexBufferPtr;
};
