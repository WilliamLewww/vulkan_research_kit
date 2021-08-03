#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

#include <vector>

class Framebuffer : public Component {
private:
  VkFramebuffer framebufferHandle;

  VkDevice* deviceHandlePtr;

  VkFramebufferCreateInfo framebufferCreateInfo;
public:
  Framebuffer(VkDevice* deviceHandlePtr,
      VkRenderPass* renderPassHandlePtr, 
      std::vector<VkImageView>* attachmentImageViewHandleListPtr,
      VkFramebufferCreateFlags framebufferCreateFlags,
      uint32_t width, uint32_t height, uint32_t layers);

  ~Framebuffer();

  bool activate();
};