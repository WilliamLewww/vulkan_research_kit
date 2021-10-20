#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class PipelineLayout {
public:
  PipelineLayout(
      VkDevice &deviceHandleRef,
      std::vector<VkDescriptorSetLayout> descriptorSetLayoutHandleList,
      std::vector<VkPushConstantRange> pushConstantRangeList);

  ~PipelineLayout();

  VkPipelineLayout &getPipelineLayoutHandleRef();

private:
  VkPipelineLayout pipelineLayoutHandle;

  VkDevice &deviceHandleRef;
};