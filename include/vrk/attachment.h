#pragma once

#include <vulkan/vulkan.h>

class Attachment {
private:
  VkAttachmentDescription attachmentDescription;
public:
  Attachment(VkFormat format,
      VkSampleCountFlagBits sampleCountFlagBits,
      VkAttachmentLoadOp attachmentLoadOp,
      VkAttachmentStoreOp attachmentStoreOp,
      VkAttachmentLoadOp stencilAttachmentLoadOp,
      VkAttachmentStoreOp stencilAttachmentStoreOp,
      VkImageLayout initialImageLayout,
      VkImageLayout finalImageLayout);

  ~Attachment();

  VkAttachmentDescription getAttachmentDescription();
};