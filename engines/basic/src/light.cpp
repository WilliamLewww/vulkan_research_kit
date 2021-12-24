#include "basic/light.h"
#include "basic/engine.h"

Light::Light(std::shared_ptr<Engine> enginePtr,
             std::shared_ptr<Buffer> lightsBufferPtr, uint32_t lightIndex,
             std::string lightName, LightType lightType)
    : enginePtr(enginePtr), lightsBufferPtr(lightsBufferPtr),
      lightIndex(lightIndex), lightName(lightName) {

  this->lightShaderStructure = {
      .position = {0, 0, 0, 1}, .direction = {0, -1, 0, 0}, .type = lightType};

  this->lightDescriptorBufferInfoPtr =
      std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo{
          .buffer = lightsBufferPtr->getBufferHandleRef(),
          .offset = lightIndex * sizeof(LightShaderStructure),
          .range = sizeof(LightShaderStructure)});

  void *hostLightsBuffer;
  this->lightsBufferPtr->mapMemory(&hostLightsBuffer, 0,
                                   16 * sizeof(LightShaderStructure));
  memcpy(&((LightShaderStructure *)hostLightsBuffer)[lightIndex],
         &this->lightShaderStructure, sizeof(LightShaderStructure));
  this->lightsBufferPtr->unmapMemory();
}

Light::~Light() {}

void Light::setPosition(float x, float y, float z) {
  this->lightShaderStructure.position[0] = x;
  this->lightShaderStructure.position[1] = y;
  this->lightShaderStructure.position[2] = z;

  void *hostLightsBuffer;
  this->lightsBufferPtr->mapMemory(&hostLightsBuffer, 0,
                                   16 * sizeof(LightShaderStructure));
  memcpy(&((LightShaderStructure *)hostLightsBuffer)[lightIndex],
         &this->lightShaderStructure, sizeof(LightShaderStructure));
  this->lightsBufferPtr->unmapMemory();
}

void Light::setDirection(float x, float y, float z) {
  this->lightShaderStructure.direction[0] = x;
  this->lightShaderStructure.direction[1] = y;
  this->lightShaderStructure.direction[2] = z;

  void *hostLightsBuffer;
  this->lightsBufferPtr->mapMemory(&hostLightsBuffer, 0,
                                   16 * sizeof(LightShaderStructure));
  memcpy(&((LightShaderStructure *)hostLightsBuffer)[lightIndex],
         &this->lightShaderStructure, sizeof(LightShaderStructure));
  this->lightsBufferPtr->unmapMemory();
}

bool Light::getIsLightBufferDirty() { return this->isLightBufferDirty; }

void Light::resetIsLightBufferDirty() { this->isLightBufferDirty = false; }

std::shared_ptr<VkDescriptorBufferInfo>
Light::getLightDescriptorBufferInfoPtr() {
  return this->lightDescriptorBufferInfoPtr;
}

uint32_t Light::getLightIndex() { return this->lightIndex; }
