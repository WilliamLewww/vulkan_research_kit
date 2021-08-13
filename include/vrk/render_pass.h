#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class RenderPass {
public:
  RenderPass(VkDevice& deviceHandleRef,
      VkRenderPassCreateFlagBits renderPassCreateFlagBits,
      std::vector<VkAttachmentDescription> attachmentDescriptionList,
      std::vector<VkSubpassDescription> subpassDescriptionList,
      std::vector<VkSubpassDependency> subpassDependencyList);

  ~RenderPass();
private:
  VkRenderPass renderPassHandle;

  VkDevice& deviceHandleRef;
};