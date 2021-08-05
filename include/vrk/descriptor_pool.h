#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

#include <vector>

class DescriptorPool : public Component {
private:
  VkDescriptorPool descriptorPoolHandle;

  VkDevice* deviceHandlePtr;

  std::vector<VkDescriptorPoolSize> descriptorPoolSizeList;

  VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
public:
  DescriptorPool(VkDevice* deviceHandlePtr,
      VkDescriptorPoolCreateFlags descriptorPoolCreateFlags,
      uint32_t maxSets, VkDescriptorType initialDescriptorType,
      uint32_t initialDescriptorCount);

  ~DescriptorPool();

  void addDescriptorPoolSize(VkDescriptorType descriptorType,
      uint32_t descriptorCount);

  bool activate();
};