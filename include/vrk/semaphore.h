#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

class Semaphore {
public:
  Semaphore(VkDevice& deviceHandleRef,
      VkSemaphoreCreateFlags semaphoreCreateFlags);

  ~Semaphore();

  VkSemaphore& getSemaphoreHandleRef();
private:
  VkSemaphore semaphoreHandle;

  VkDevice& deviceHandleRef;
};