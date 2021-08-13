#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class Framebuffer {
public:
  Framebuffer(VkDevice& deviceHandleRef,
      VkRenderPass& renderPassHandleRef,
      std::vector<VkImageView> imageViewHandleList,
      VkFramebufferCreateFlags framebufferCreateFlags,
      uint32_t width,
      uint32_t height,
      uint32_t layers);

  ~Framebuffer();
private:
  VkFramebuffer framebufferHandle;

  VkDevice& deviceHandleRef;
};