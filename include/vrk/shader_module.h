#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

#include <vector>

class ShaderModule : public Component {
private:
  VkShaderModule shaderModuleHandle;

  VkDevice* deviceHandlePtr;

  std::vector<uint32_t> code;
public:
  ShaderModule(VkDevice* deviceHandlePtr, std::vector<uint32_t> code);

  ~ShaderModule();

  bool activate();
};