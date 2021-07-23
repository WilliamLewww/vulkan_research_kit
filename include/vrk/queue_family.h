#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class QueueFamily {
private:
  std::vector<VkQueue> queueHandleList;
public:
  QueueFamily();
  ~QueueFamily();
};