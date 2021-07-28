#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

class Fence : public Component {
private:
  VkFence fenceHandle;

  VkDevice* deviceHandlePtr;

  VkFenceCreateFlags fenceCreateFlags;
public:
  Fence(VkDevice* deviceHandlePtr, VkFenceCreateFlags fenceCreateFlags = 0);

  ~Fence();

  bool activate();

  VkFence* getFenceHandlePtr();

  friend std::ostream& operator<<(std::ostream& os,
      const Fence& fence);
};