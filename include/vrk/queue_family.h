#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

#include <vector>

class QueueFamily : public Component {
private:
  std::vector<VkQueue> queueHandleList;

  float queuePriority;

  VkDeviceQueueCreateInfo deviceQueueCreateInfo;
public:
  QueueFamily(uint32_t queueFamilyIndex, uint32_t queueCount,
      float queuePriority = 1.0f);

  ~QueueFamily();

  uint32_t getFamilyIndex();

  uint32_t getQueueCount();

  VkDeviceQueueCreateInfo getDeviceQueueCreateInfo();

  bool activate(VkDevice* deviceHandlePtr);

  VkQueue* getQueueHandlePtr(uint32_t index);
};