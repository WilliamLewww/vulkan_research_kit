#pragma once
#include "basic/material.h"

#include <string>

class Model {
public:
  Model(std::string name, std::string modelPath,
        std::shared_ptr<Material> materialPtr);

  ~Model();

private:
  std::string name;

  std::shared_ptr<Material> materialPtr;
};
