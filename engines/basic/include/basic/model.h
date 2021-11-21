#pragma once
#include "basic/material.h"

#include <vrk/device.h>

#include <string>

class Model {
public:
  Model(std::shared_ptr<Device> devicePtr, std::string modelName,
        std::string modelPath, std::shared_ptr<Material> materialPtr);

  ~Model();

private:
  std::string modelName;

  std::shared_ptr<Material> materialPtr;
};
