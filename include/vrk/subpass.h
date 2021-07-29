#pragma once

#include "vrk/component.h"

#include <vulkan/vulkan.h>

class Subpass {
private:
  VkSubpassDescription subpassDescription;
public:
  Subpass(VkPipelineBindPoint pipelineBindPoint);

  ~Subpass();
};