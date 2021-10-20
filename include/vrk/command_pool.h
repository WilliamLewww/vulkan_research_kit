#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

class CommandPool {
public:
  CommandPool(VkDevice &deviceHandleRef,
              VkCommandPoolCreateFlags commandPoolCreateFlags,
              uint32_t queueFamilyIndex);

  ~CommandPool();

  void reset(VkCommandPoolResetFlags commandPoolResetFlags);

  VkCommandPool &getCommandPoolHandleRef();

private:
  VkCommandPool commandPoolHandle;

  VkDevice &deviceHandleRef;
};