#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

class Fence {
public:
  Fence(VkDevice &deviceHandleRef, VkFenceCreateFlagBits fenceCreateFlagBits);

  ~Fence();

  bool getSignaled();

  bool waitForSignal(uint32_t timeout);

  void reset();

  VkFence &getFenceHandleRef();

private:
  VkFence fenceHandle;

  VkDevice &deviceHandleRef;
};