#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

class Semaphore : public Component {
private:
  VkSemaphore semaphoreHandle;

  VkDevice* deviceHandlePtr;
public:
  Semaphore(VkDevice* deviceHandlePtr);

  ~Semaphore();

  bool activate();

  VkSemaphore* getSemaphoreHandlePtr();
};