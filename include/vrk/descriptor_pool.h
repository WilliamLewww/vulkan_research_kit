#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <tuple>

class DescriptorPool : public Component {
private:
  VkDescriptorPool descriptorPoolHandle;

  VkDevice* deviceHandlePtr;

  std::vector<VkDescriptorPoolSize> descriptorPoolSizeList;

  VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;

  struct DescriptorPoolSize {
    VkDescriptorType type;
    uint32_t descriptorCount;
  };

public:
  DescriptorPool(VkDevice* deviceHandlePtr,
      VkDescriptorPoolCreateFlags descriptorPoolCreateFlags,
      uint32_t maxSets,
      std::vector<DescriptorPoolSize> descriptorPoolSizeList);

  ~DescriptorPool();

  bool activate();
};