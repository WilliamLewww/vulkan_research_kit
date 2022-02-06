#pragma once

#include <vrk/buffer.h>

#include <memory>
#include <string>

class Engine;

class Light {
public:
  enum LightType { LIGHT_TYPE_DIRECTIONAL, LIGHT_TYPE_POINT };

  struct alignas(64) LightShaderStructure {
    alignas(16) float position[4];
    alignas(16) float direction[4];
    alignas(4) int type;
  };

  Light(std::shared_ptr<Engine> enginePtr,
        std::shared_ptr<Buffer> lightsBufferPtr, uint32_t lightIndex,
        std::string lightName, LightType lightType);

  ~Light();

  void setPosition(float x, float y, float z);

  void updatePosition(float x, float y, float z);

  void setDirection(float x, float y, float z);

  void updateDirection(float x, float y, float z);

  uint32_t getLightIndex();

  bool getIsLightBufferDirty();

  void resetIsLightBufferDirty();

  std::shared_ptr<VkDescriptorBufferInfo> getLightDescriptorBufferInfoPtr();

private:
  std::shared_ptr<Engine> enginePtr;

  std::shared_ptr<Buffer> lightsBufferPtr;

  uint32_t lightIndex;

  std::string lightName;

  bool isLightBufferDirty;

  LightShaderStructure lightShaderStructure;

  std::shared_ptr<VkDescriptorBufferInfo> lightDescriptorBufferInfoPtr;
};
