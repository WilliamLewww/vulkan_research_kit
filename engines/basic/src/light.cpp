#include "basic/light.h"
#include "basic/engine.h"

Light::Light(std::shared_ptr<Engine> enginePtr,
             std::shared_ptr<Buffer> lightsBufferPtr, uint32_t lightIndex,
             std::string lightName, LightType lightType)
    : enginePtr(enginePtr), lightsBufferPtr(lightsBufferPtr),
      lightIndex(lightIndex), lightName(lightName) {

  void *hostLightsBuffer;
  this->lightsBufferPtr->mapMemory(&hostLightsBuffer, 0,
                                   16 * sizeof(LightShaderStructure));
  memcpy(&((LightShaderStructure *)hostLightsBuffer)[lightIndex].type,
         &this->lightShaderStructure.type, 1 * sizeof(uint32_t));
  this->lightsBufferPtr->unmapMemory();
}

Light::~Light() {}
