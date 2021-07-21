#pragma once

#include "vrk/instance.h"
#include "vrk/device.h"

class Engine {
private:
  Instance* instance;
  Device* device;
public:
  Engine();
  ~Engine();
};
