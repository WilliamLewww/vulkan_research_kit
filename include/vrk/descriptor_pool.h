#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class DescriptorPool {
public:
  DescriptorPool(VkDevice &deviceHandleRef,
                 VkDescriptorPoolCreateFlags descriptorPoolCreateFlags,
                 uint32_t maxSets,
                 std::vector<VkDescriptorPoolSize> descriptorPoolSizeList);

  ~DescriptorPool();

  VkDescriptorPool &getDescriptorPoolHandleRef();

private:
  VkDescriptorPool descriptorPoolHandle;

  VkDevice &deviceHandleRef;
};