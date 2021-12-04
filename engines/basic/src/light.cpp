#include "basic/light.h"
#include "basic/engine.h"

Light::Light(std::shared_ptr<Engine> enginePtr, std::string lightName)
    : enginePtr(enginePtr), lightName(lightName) {}

Light::~Light() {}
