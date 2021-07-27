#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

class Fence {
private:
  bool isActive;

  VkFence fenceHandle;

  VkDevice* deviceHandlePtr;

  VkFenceCreateFlags fenceCreateFlags;
public:
  Fence(VkDevice* deviceHandlePtr, VkFenceCreateFlags fenceCreateFlags = 0);

  ~Fence();

  void activate();

  VkFence* getFenceHandlePtr();

  friend std::ostream& operator<<(std::ostream& os,
      const Fence& fence);
};