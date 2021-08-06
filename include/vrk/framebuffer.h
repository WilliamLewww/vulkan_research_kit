#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

#include <vector>

class Framebuffer : public Component {
private:
  VkFramebuffer framebufferHandle;

  VkDevice* deviceHandlePtr;

  std::vector<VkImageView> attachmentImageViewHandleList;

  VkFramebufferCreateInfo framebufferCreateInfo;
public:
  Framebuffer(VkDevice* deviceHandlePtr,
      VkRenderPass* renderPassHandlePtr,
      VkImageView* initialAttachmentImageViewHandlePtr,
      VkFramebufferCreateFlags framebufferCreateFlags,
      uint32_t width,
      uint32_t height,
      uint32_t layers);

  ~Framebuffer();

  void addAttachmentImageViewHandle(VkImageView* attachmentImageViewHandlePtr);

  bool activate();
};