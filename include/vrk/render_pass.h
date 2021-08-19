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

  void beginRenderPassCmd(VkCommandBuffer& commandBufferHandleRef,
      VkFramebuffer& framebufferHandleRef,
      VkRect2D renderAreaRect2D,
      std::vector<VkClearValue> clearValueList,
      VkSubpassContents subpassContents);

  void endRenderPassCmd(VkCommandBuffer& commandBufferHandleRef);

  void drawIndexedCmd(VkCommandBuffer& commandBufferHandleRef,
      uint32_t indexCount,
      uint32_t instanceCount,
      uint32_t firstIndex,
      uint32_t vertexOffset,
      uint32_t firstInstance);

  VkRenderPass& getRenderPassHandleRef();
private:
  VkRenderPass renderPassHandle;

  VkDevice& deviceHandleRef;
};