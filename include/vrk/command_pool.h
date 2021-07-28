#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

class CommandPool : public Component {
private:
  VkCommandPool commandPoolHandle;

  VkDevice* deviceHandlePtr;

  uint32_t queueFamilyIndex;
public:
  CommandPool(VkDevice* deviceHandlePtr, uint32_t queueFamilyIndex);

  ~CommandPool();

  bool activate();

  void reset(VkCommandPoolResetFlags commandPoolResetFlags = 0);

  VkCommandPool* getCommandPoolHandlePtr();

  friend std::ostream& operator<<(std::ostream& os,
      const CommandPool& commandPool);
};