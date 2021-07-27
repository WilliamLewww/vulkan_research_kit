#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

class Semaphore {
private:
  bool isActive;

  VkSemaphore semaphoreHandle;

  VkDevice* deviceHandlePtr;
public:
  Semaphore(VkDevice* deviceHandlePtr);

  ~Semaphore();

  void activate();

  VkSemaphore* getSemaphoreHandlePtr();

  friend std::ostream& operator<<(std::ostream& os,
      const Semaphore& semaphore);
};