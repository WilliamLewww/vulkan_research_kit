#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class ShaderModule {
public:
  ShaderModule(VkDevice& deviceHandleRef, std::vector<uint32_t> code);

  ~ShaderModule();
private:
  VkShaderModule shaderModuleHandle;

  VkDevice& deviceHandleRef;
};