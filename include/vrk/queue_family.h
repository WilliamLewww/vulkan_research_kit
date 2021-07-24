#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class QueueFamily {
private:
  std::vector<VkQueue> queueHandleList;

  uint32_t queueFamilyIndex;
  float queuePriority;

  VkDeviceQueueCreateInfo deviceQueueCreateInfo;
public:
  QueueFamily(uint32_t queueFamilyIndex, uint32_t queueCount,
      float queuePriority = 1.0f);

  ~QueueFamily();

  VkDeviceQueueCreateInfo getDeviceQueueCreateInfo();
};