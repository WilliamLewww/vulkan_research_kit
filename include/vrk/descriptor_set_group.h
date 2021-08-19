#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class DescriptorSetGroup {
public:
  DescriptorSetGroup(VkDevice& deviceHandleRef,
      VkDescriptorPool& descriptorPoolHandleRef,
      std::vector<VkDescriptorSetLayout> descriptorSetLayoutList);

  ~DescriptorSetGroup();
private:
  std::vector<VkDescriptorSet> descriptorSetHandleList;

  VkDevice& deviceHandleRef;

  VkDescriptorPool& descriptorPoolHandleRef;
};