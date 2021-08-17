#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class DescriptorSetLayout {
public:
  DescriptorSetLayout(VkDevice& deviceHandleRef,
      VkDescriptorSetLayoutCreateFlags descriptorSetLayoutCreateFlags,
      std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindingList);

  ~DescriptorSetLayout();
private:
  VkDescriptorSetLayout descriptorSetLayoutHandle;

  VkDevice& deviceHandleRef;
};