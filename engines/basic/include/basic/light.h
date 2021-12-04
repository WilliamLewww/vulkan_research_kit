#pragma once

#include <vrk/buffer.h>

#include <memory>
#include <string>

class Engine;

class Light {
public:
  struct LightShaderStructure {};

  Light(std::shared_ptr<Engine> enginePtr, std::string lightName);

  ~Light();

private:
  std::shared_ptr<Engine> enginePtr;

  std::string lightName;

  std::shared_ptr<Buffer> lightBufferPtr;

  std::shared_ptr<VkDescriptorBufferInfo> lightDescriptorBufferInfoPtr;
};
