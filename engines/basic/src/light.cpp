#include "basic/light.h"
#include "basic/engine.h"

Light::Light(std::shared_ptr<Engine> enginePtr,
             std::shared_ptr<Buffer> lightsBufferPtr, uint32_t lightIndex,
             std::string lightName, LightType lightType)
    : enginePtr(enginePtr), lightsBufferPtr(lightsBufferPtr),
      lightIndex(lightIndex), lightName(lightName) {

  this->lightShaderStructure = {.type = lightType};

  this->lightDescriptorBufferInfoPtr =
      std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo{
          .buffer = lightsBufferPtr->getBufferHandleRef(),
          .offset = lightIndex * sizeof(LightShaderStructure),
          .range = sizeof(LightShaderStructure)});

  void *hostLightsBuffer;
  this->lightsBufferPtr->mapMemory(&hostLightsBuffer, 0,
                                   16 * sizeof(LightShaderStructure));
  memcpy(&((LightShaderStructure *)hostLightsBuffer)[lightIndex].type,
         &this->lightShaderStructure.type, 1 * sizeof(int));
  this->lightsBufferPtr->unmapMemory();
}

Light::~Light() {}

std::shared_ptr<VkDescriptorBufferInfo>
Light::getLightDescriptorBufferInfoPtr() {
  return this->lightDescriptorBufferInfoPtr;
}
