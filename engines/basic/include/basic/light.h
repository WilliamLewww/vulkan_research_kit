#pragma once

#include <vrk/buffer.h>

#include <memory>
#include <string>

class Engine;

class Light {
public:
  enum class LightType { Directional, Point };

  struct LightShaderStructure {
    alignas(16) float position[4];
    alignas(16) float direction[4];
    alignas(4) uint32_t type;
  };

  Light(std::shared_ptr<Engine> enginePtr,
        std::shared_ptr<Buffer> lightsBufferPtr, uint32_t lightIndex,
        std::string lightName, LightType lightType);

  ~Light();

private:
  std::shared_ptr<Engine> enginePtr;

  std::shared_ptr<Buffer> lightsBufferPtr;

  uint32_t lightIndex;

  std::string lightName;

  LightShaderStructure lightShaderStructure;
};
