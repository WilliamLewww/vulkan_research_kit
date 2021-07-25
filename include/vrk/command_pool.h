#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

class CommandPool {
private:
  bool isActive;

  VkCommandPool commandPoolHandle;

  VkDevice* deviceHandlePtr;

  uint32_t queueFamilyIndex;
public:
  CommandPool(VkDevice* deviceHandlePtr, uint32_t queueFamilyIndex);

  ~CommandPool();

  void activate();

  friend std::ostream& operator<<(std::ostream& os,
      const CommandPool& commandPool);
};