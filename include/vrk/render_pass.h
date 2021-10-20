#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class RenderPass {
public:
  RenderPass(VkDevice &deviceHandleRef,
             VkRenderPassCreateFlagBits renderPassCreateFlagBits,
             std::vector<VkAttachmentDescription> attachmentDescriptionList,
             std::vector<VkSubpassDescription> subpassDescriptionList,
             std::vector<VkSubpassDependency> subpassDependencyList);

  ~RenderPass();

  void beginRenderPassCmd(VkCommandBuffer &commandBufferHandleRef,
                          VkFramebuffer &framebufferHandleRef,
                          VkRect2D renderAreaRect2D,
                          std::vector<VkClearValue> clearValueList,
                          VkSubpassContents subpassContents);

  void endRenderPassCmd(VkCommandBuffer &commandBufferHandleRef);

  VkRenderPass &getRenderPassHandleRef();

private:
  VkRenderPass renderPassHandle;

  VkDevice &deviceHandleRef;
};