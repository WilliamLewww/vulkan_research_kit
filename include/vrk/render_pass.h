#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

class RenderPass : public Component {
private:
public:
  RenderPass();

  ~RenderPass();

  bool activate();
};